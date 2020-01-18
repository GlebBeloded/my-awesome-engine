#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine.hpp"
#include "shaders.hpp"

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <android/log.h>
#define GL_GLES_PROTOTYPES 1
#define glActiveTexture_ glActiveTexture
#else
#include "glad.h"
#endif
#include <SDL.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

template <typename T>
static void load_gl_func(const char* func_name, T& result) {
    void* gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer) {
        throw std::runtime_error(std::string("can't load GL function") +
                                 func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
}

void gl_error_check() {

    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
                break;
        }
        assert(false);
    }
}
namespace eng {

membuf load_file(std::string_view path) {
    SDL_RWops* io = SDL_RWFromFile(path.data(), "rb");
    if (nullptr == io) {
        throw std::runtime_error("can't load file: " + std::string(path));
    }

    Sint64 file_size = io->size(io);
    if (-1 == file_size) {
        throw std::runtime_error("can't determine size of file: " +
                                 std::string(path));
    }
    size_t                  size = static_cast<size_t>(file_size);
    std::unique_ptr<char[]> mem  = std::make_unique<char[]>(size);

    size_t num_readed_objects = io->read(io, mem.get(), size, 1);
    if (num_readed_objects != 1) {
        throw std::runtime_error("can't read all content from file: " +
                                 std::string(path));
    }

    if (0 != io->close(io)) {
        throw std::runtime_error("failed close file: " + std::string(path));
    }
    return membuf(std::move(mem), size);
}

static std::string_view get_sound_format_name(uint16_t format_value) {
    static const std::map<int, std::string_view> format = {
        { AUDIO_U8, "AUDIO_U8" },         { AUDIO_S8, "AUDIO_S8" },
        { AUDIO_U16LSB, "AUDIO_U16LSB" }, { AUDIO_S16LSB, "AUDIO_S16LSB" },
        { AUDIO_U16MSB, "AUDIO_U16MSB" }, { AUDIO_S16MSB, "AUDIO_S16MSB" },
        { AUDIO_S32LSB, "AUDIO_S32LSB" }, { AUDIO_S32MSB, "AUDIO_S32MSB" },
        { AUDIO_F32LSB, "AUDIO_F32LSB" }, { AUDIO_F32MSB, "AUDIO_F32MSB" },
    };

    auto it = format.find(format_value);
    return it->second;
}

static std::size_t get_sound_format_size(uint16_t format_value) {
    static const std::map<int, std::size_t> format = {
        { AUDIO_U8, 1 },     { AUDIO_S8, 1 },     { AUDIO_U16LSB, 2 },
        { AUDIO_S16LSB, 2 }, { AUDIO_U16MSB, 2 }, { AUDIO_S16MSB, 2 },
        { AUDIO_S32LSB, 4 }, { AUDIO_S32MSB, 4 }, { AUDIO_F32LSB, 4 },
        { AUDIO_F32MSB, 4 },
    };

    auto it = format.find(format_value);
    return it->second;
}

template <typename T>
T lerp(T a, T b, float t) {
    return a + t * (b - a);
}

rgb& rgb::operator=(const rgb& rh) {
    r = rh.r;
    g = rh.g;
    b = rh.b;
    return *this;
}

constexpr static std::array<std::string_view, 17> event_names = {
    { /// input events
      "w_pressed", "a_pressed", "s_pressed", "d_pressed", "esc_pressed",
      "w_released", "a_released", "s_released", "d_released", "esc_released" }
};

std::ostream& operator<<(std::ostream& stream, const event e) {
    std::uint32_t           value = static_cast<std::uint32_t>(e);
    constexpr std::uint32_t minimal =
        static_cast<std::uint32_t>(event::w_pressed);
    constexpr std::uint32_t maximal =
        static_cast<std::uint32_t>(event::esc_released);
    if (value >= minimal && value <= maximal) {
        stream << event_names[value];
        return stream;
    } else {
        throw std::runtime_error("too big event value");
    }
}

static std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

std::istream& operator>>(std::istream& is, vertex& v) {
    is >> v.coord;

    is >> v.color.r;
    is >> v.color.g;
    is >> v.color.b;

    is >> v.tx;
    is >> v.ty;
    return is;
}

std::istream& operator>>(std::istream& is, triangle& t) {
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

struct binding {
    constexpr binding(SDL_Keycode k, std::string_view s, event pressed,
                      event released)
        : key(k)
        , name(s)
        , event_pressed(pressed)
        , event_released(released) {}

    SDL_Keycode      key;
    std::string_view name;
    event            event_pressed;
    event            event_released;
};

constexpr std::array<binding, 5> keys{
    { { SDLK_w, "up", event::w_pressed, event::w_released },
      { SDLK_a, "left", event::a_pressed, event::a_released },
      { SDLK_s, "down", event::s_pressed, event::s_released },
      { SDLK_d, "right", event::d_pressed, event::d_released },
      { SDLK_ESCAPE, "escape", event::esc_pressed, event::esc_released } }
};

static bool check_input(const SDL_Event& e, const binding*& result) {
    using namespace std;

    const auto it = find_if(begin(keys), end(keys), [&](const binding& b) {
        return b.key == e.key.keysym.sym;
    });

    if (it != end(keys)) {
        result = &(*it);
        return true;
    }
    return false;
}

class sound_buffer_impl final : public sound_buffer {
public:
    sound_buffer_impl(std::string_view path, SDL_AudioDeviceID device,
                      SDL_AudioSpec audio_spec);
    ~sound_buffer_impl() final;

    void play(const properties prop) final {
        // Lock callback function
        SDL_LockAudioDevice(device);

        // here we can change properties
        // of sound and dont collade with multithreaded playing
        current_index = 0;
        is_playing    = true;
        is_looped     = (prop == properties::looped);

        // unlock callback for continue mixing of audio
        SDL_UnlockAudioDevice(device);
    }

    std::unique_ptr<uint8_t[]> tmp_buf;
    uint8_t*                   buffer;
    uint32_t                   length;
    uint32_t                   current_index = 0;
    SDL_AudioDeviceID          device;
    bool                       is_playing = false;
    bool                       is_looped  = false;
};

sound_buffer_impl::sound_buffer_impl(std::string_view  path,
                                     SDL_AudioDeviceID device_,
                                     SDL_AudioSpec     device_audio_spec)
    : buffer(nullptr)
    , length(0)
    , device(device_) {
    SDL_RWops* file = SDL_RWFromFile(path.data(), "rb");
    if (file == nullptr) {
        throw std::runtime_error(std::string("can't open audio file: ") +
                                 path.data());
    }

    // freq, format, channels, and samples - used by SDL_LoadWAV_RW
    SDL_AudioSpec file_audio_spec;

    if (nullptr ==
        SDL_LoadWAV_RW(file, 1, &file_audio_spec, &buffer, &length)) {
        throw std::runtime_error(std::string("can't load wav: ") + path.data());
    }

    std::cout << "audio format for: " << path << '\n'
              << "format: " << get_sound_format_name(file_audio_spec.format)
              << '\n'
              << "sample_size: "
              << get_sound_format_size(file_audio_spec.format) << '\n'
              << "channels: " << static_cast<uint32_t>(file_audio_spec.channels)
              << '\n'
              << "frequency: " << file_audio_spec.freq << '\n'
              << "length: " << length << '\n'
              << "time: "
              << static_cast<double>(length) /
                     (file_audio_spec.channels * file_audio_spec.freq *
                      get_sound_format_size(file_audio_spec.format))
              << "sec" << std::endl;

    if (file_audio_spec.channels != device_audio_spec.channels ||
        file_audio_spec.format != device_audio_spec.format ||
        file_audio_spec.freq != device_audio_spec.freq) {
        SDL_AudioCVT cvt;
        SDL_BuildAudioCVT(&cvt, file_audio_spec.format,
                          file_audio_spec.channels, file_audio_spec.freq,
                          device_audio_spec.format, device_audio_spec.channels,
                          device_audio_spec.freq);
        SDL_assert(cvt.needed); // obviously, this one is always needed.
        // read your data into cvt.buf here.
        cvt.len = length;
        // we have to make buffer for inplace conversion
        tmp_buf.reset(new uint8_t[cvt.len * cvt.len_mult]);
        uint8_t* buf = tmp_buf.get();
        std::copy_n(buffer, length, buf);
        cvt.buf = buf;
        if (0 != SDL_ConvertAudio(&cvt)) {
            std::cout << "failed to convert audio from file: " << path
                      << " to audio device format" << std::endl;
        }
        // cvt.buf has cvt.len_cvt bytes of converted data now.
        SDL_FreeWAV(buffer);
        buffer = tmp_buf.get();
        length = cvt.len_cvt;
    }
}

sound_buffer::~sound_buffer() {}

sound_buffer_impl::~sound_buffer_impl() {
    if (!tmp_buf) {
        SDL_FreeWAV(buffer);
    }
    buffer = nullptr;
    length = 0;
}

class sdl_engine final : public engine {
public:
    sdl_engine(const std::string_view& assets_path);
    virtual ~sdl_engine();
    bool  read_input(input_data&) final;
    void  render_line(const line& l) override;
    void  render_triangle(const triangle& t) override;
    void  swap_buffers() override;
    float time_from_init() override;
    uint  load_texture(std::string_view path, int width, int height,
                       int nrChannels, int texture_mode) override;

    virtual sound_buffer* create_sound_buffer(std::string_view path) override;
    virtual void          destroy_sound_buffer(sound_buffer*) override;
    std::vector<sound_buffer_impl*> sounds;

private:
    static void   audio_callback(void*, uint8_t*, int);
    SDL_GLContext gl_context = nullptr;
    // map of strings mapped to the program IDs
    std::map<std::string, GLuint> shaders;
    SDL_Window*                   window = nullptr;

    void init_SDL();
    void init_SDL_audio();

    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec     audio_device_spec;
};

engine* new_sdl_engine(const std::string_view& assets_path) {
    return new sdl_engine(assets_path);
}

void sdl_engine::init_SDL_audio() {

    audio_device_spec.freq     = 48000;
    audio_device_spec.format   = AUDIO_S16LSB;
    audio_device_spec.channels = 2;
    audio_device_spec.samples  = 1024; // must be power of 2
    audio_device_spec.callback = sdl_engine::audio_callback;
    audio_device_spec.userdata = this;

    const int num_audio_drivers = SDL_GetNumAudioDrivers();
    for (int i = 0; i < num_audio_drivers; ++i) {
        std::cout << "audio_driver #:" << i << " " << SDL_GetAudioDriver(i)
                  << '\n';
    }
    std::cout << std::flush;

    const char* default_audio_device_name = nullptr;

    const int num_audio_devices = SDL_GetNumAudioDevices(SDL_FALSE);
    if (num_audio_devices > 0) {
        default_audio_device_name = SDL_GetAudioDeviceName(0, SDL_FALSE);
        for (int i = 0; i < num_audio_devices; ++i) {
            std::cout << "audio device #" << i << ": "
                      << SDL_GetAudioDeviceName(i, SDL_FALSE) << '\n';
        }
    }
    std::cout << std::flush;

    audio_device =
        SDL_OpenAudioDevice(default_audio_device_name, 0, &audio_device_spec,
                            nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (audio_device == 0) {
        std::cerr << "failed open audio device: " << SDL_GetError();
        throw std::runtime_error("audio failed");
    } else {
        std::cout << "audio device selected: " << default_audio_device_name
                  << '\n'
                  << "freq: " << audio_device_spec.freq << '\n'
                  << "format: "
                  << get_sound_format_name(audio_device_spec.format) << '\n'
                  << "channels: "
                  << static_cast<uint32_t>(audio_device_spec.channels) << '\n'
                  << "samples: " << audio_device_spec.samples << '\n'
                  << std::flush;

        // unpause device
        SDL_PauseAudioDevice(audio_device, SDL_FALSE);
    }
}

void sdl_engine::init_SDL() {
    using namespace std;

    SDL_version compiled = { 0, 0, 0 };
    SDL_version linked   = { 0, 0, 0 };

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    if (SDL_COMPILEDVERSION !=
        SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
        std::cerr << "warning: SDL2 compiled and linked version mismatch: "
                  << compiled << " " << linked << endl;
    }

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0) {
        const char* err_message = SDL_GetError();
        std::cerr << "error: failed call SDL_Init: " << err_message << endl;
        throw std::runtime_error("Failed to init SDL");
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 300, 600,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr) {
        const char* err_message = SDL_GetError();
        std::cerr << "error: failed call SDL_CreateWindow: " << err_message
                  << endl;
        SDL_Quit();
        throw std::runtime_error("Failed to create a window");
    }

    int gl_major_ver = 2;
    int gl_minor_ver = 1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        gl_context = SDL_GL_CreateContext(window);
    }

    assert(gl_context != nullptr);

    int result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
    assert(result == 0);

    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
    assert(result == 0);

    if (gl_major_ver <= 2 && gl_minor_ver < 1) {
        std::clog << "current context opengl version: " << gl_major_ver << '.'
                  << gl_minor_ver << '\n'
                  << "need opengl version at least: 2.1\n"
                  << std::flush;
        throw std::runtime_error("opengl version too low");
    }
}

sdl_engine::sdl_engine(const std::string_view& assets_path) {
    using namespace std;

    // Create SDL window with opengl context
    init_SDL();

    init_SDL_audio();

#ifndef __ANDROID__
    // Get opengl functions from GLAD
    if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0) {
        throw std::logic_error("error: failed to initialize glad");
    }
#endif

    // create vertex and fragment shader for drawning triangles
    const std::vector<std::string_view> triangle_attributes{ "a_position",
                                                             "a_color",
                                                             "a_tex_coord" };

    std::string shaders_dir{ assets_path.data() };
    shaders_dir.append("shaders/");
    std::string vertex_shader{ shaders_dir };
    vertex_shader.append("triangle.vert");
    std::string fragment_shader{ shaders_dir };
    fragment_shader.append("triangle.frag");

#ifdef __ANDROID__
    vertex_shader   = "triangle.vert";
    fragment_shader = "triangle.frag";
#endif

    shaders["triangle"] = create_shader_program(
        vertex_shader.data(), fragment_shader.data(), triangle_attributes);

    gl_error_check();
    if (0 == shaders["triangle"]) {
        throw std::runtime_error("failed to create gl program");
    }

    glValidateProgram(shaders["triangle"]);
    gl_error_check();

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    gl_error_check();
}

sdl_engine::~sdl_engine() {
    SDL_GL_DeleteContext(gl_context);
}

/// pool event from input queue
/// return true if more events in queue
bool sdl_engine::read_input(input_data& input) {
    using namespace std;
    // collect all events from SDL
    SDL_Event sdl_event;

    if (SDL_PollEvent(&sdl_event)) {
        const binding* binding = nullptr;

        if (sdl_event.type == SDL_QUIT) {
            input.eventType = event_type::key;
            input.key_event.emplace(event::esc_pressed);
            return true;
        } else if (sdl_event.type == SDL_KEYDOWN) {
            if (check_input(sdl_event, binding)) {
                input.eventType = event_type::key;
                input.key_event.emplace(binding->event_pressed);
                return true;
            }
        } else if (sdl_event.type == SDL_KEYUP) {
            if (check_input(sdl_event, binding)) {
                input.eventType = event_type::key;
                input.key_event.emplace(binding->event_released);
                return true;
            }
        }
        if (sdl_event.type == SDL_FINGERDOWN) {
            input.eventType = event_type::touch;
            input.touch_event.emplace(touch::finger_down);
            input.f_coord.emplace(std::pair<float, float>{
                sdl_event.tfinger.x, sdl_event.tfinger.y });
            return true;
        }
        if (sdl_event.type == SDL_FINGERUP) {
            input.eventType = event_type::touch;
            input.touch_event.emplace(touch::finger_up);
            input.f_coord.emplace(std::pair<float, float>{
                sdl_event.tfinger.x, sdl_event.tfinger.y });
            return true;
        }
    }

    return false;
}

void sdl_engine::render_triangle(const triangle& t) {
    auto program_id = shaders["triangle"];
    glUseProgram(program_id);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), &t.v[0]);
    gl_error_check();

    glEnableVertexAttribArray(0);
    gl_error_check();

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          &t.v[0].color.r);
    gl_error_check();

    glEnableVertexAttribArray(1);
    gl_error_check();

    glValidateProgram(program_id);
    gl_error_check();

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &t.v[0].tx);
    gl_error_check();

    glEnableVertexAttribArray(2);
    gl_error_check();

    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &validate_status);
    gl_error_check();
    if (validate_status == GL_FALSE) {
        GLint infoLen = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id, infoLen, nullptr, infoLog.data());
        gl_error_check();
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    gl_error_check();
}

void sdl_engine::render_line(const line& l) {
    auto program_id = shaders["line"];
    glUseProgram(program_id);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, &l);

    glEnableVertexAttribArray(0);
    gl_error_check();

    glValidateProgram(program_id);
    gl_error_check();

    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &validate_status);
    gl_error_check();
    if (validate_status == GL_FALSE) {
        GLint infoLen = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id, infoLen, nullptr, infoLog.data());
        gl_error_check();
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }

    glDrawArrays(GL_LINES, 0, 2);
    gl_error_check();
}

void sdl_engine::swap_buffers() {
    SDL_GL_SwapWindow(window);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl_error_check();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_error_check();
}

vertex blend(const vertex& a, const vertex& b, const float t) {
    vertex r;
    r.coord.x = lerp(a.coord.x, b.coord.x, t);
    r.coord.y = lerp(a.coord.y, b.coord.y, t);
    r.coord.z = lerp(a.coord.z, b.coord.z, t);
    r.color.r = lerp(a.color.r, b.color.r, t);
    r.color.g = lerp(a.color.g, b.color.g, t);
    r.color.b = lerp(a.color.b, b.color.b, t);
    r.tx      = lerp(a.tx, b.tx, t);
    r.ty      = lerp(a.ty, b.ty, t);

    return r;
}

triangle blend(const triangle& tl, const triangle& tr, const float a) {
    triangle r;
    r.v[0] = blend(tl.v[0], tr.v[0], a);
    r.v[1] = blend(tl.v[1], tr.v[1], a);
    r.v[2] = blend(tl.v[2], tr.v[2], a);
    return r;
}

float sdl_engine::time_from_init() {
    std::uint32_t ms_from_library_initialization = SDL_GetTicks();
    float         seconds = ms_from_library_initialization * 0.001f;
    return seconds;
}

uint sdl_engine::load_texture(std::string_view path, int width, int height,
                              int nrChannels, int texture_mode) {

    auto texture_file = load_file(path);

    stbi_uc* data = nullptr;

    data = stbi_load_from_memory(
        reinterpret_cast<unsigned char*>(texture_file.begin()),
        texture_file.size(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        throw std::runtime_error("error reading texture");
    }

    GLuint texture;
    glGenTextures(1, &texture);
    gl_error_check();

    glBindTexture(GL_TEXTURE_2D, texture);
    gl_error_check();

    glTexImage2D(GL_TEXTURE_2D, 0, texture_mode, width, height, 0, texture_mode,
                 GL_UNSIGNED_BYTE, data);
    gl_error_check();

    glGenerateMipmap(GL_TEXTURE_2D);
    gl_error_check();

    stbi_image_free(data);

    return texture;
}

sound_buffer* sdl_engine::create_sound_buffer(std::string_view path) {
    SDL_LockAudioDevice(audio_device); // TODO fix lock only push_back
    sound_buffer_impl* s =
        new sound_buffer_impl(path, audio_device, audio_device_spec);
    sounds.push_back(s);
    SDL_UnlockAudioDevice(audio_device);
    return s;
}
void sdl_engine::destroy_sound_buffer(sound_buffer* sound) {
    // TODO FIXME first remove from sounds collection
    delete sound;
}

void sdl_engine::audio_callback(void* engine_ptr, uint8_t* stream,
                                int stream_size) {
    // no sound default
    std::fill_n(stream, stream_size, '\0');

    sdl_engine* e = static_cast<sdl_engine*>(engine_ptr);

    for (sound_buffer_impl* snd : e->sounds) {
        if (snd->is_playing) {
            uint32_t rest         = snd->length - snd->current_index;
            uint8_t* current_buff = &snd->buffer[snd->current_index];

            if (rest <= static_cast<uint32_t>(stream_size)) {
                // copy rest to buffer
                SDL_MixAudioFormat(stream, current_buff,
                                   e->audio_device_spec.format, rest,
                                   SDL_MIX_MAXVOLUME);
                snd->current_index += rest;
            } else {
                SDL_MixAudioFormat(
                    stream, current_buff, e->audio_device_spec.format,
                    static_cast<uint32_t>(stream_size), SDL_MIX_MAXVOLUME);
                snd->current_index += static_cast<uint32_t>(stream_size);
            }

            if (snd->current_index == snd->length) {
                if (snd->is_looped) {
                    // start from begining
                    snd->current_index = 0;
                } else {
                    snd->is_playing = false;
                }
            }
        }
    }
}

} // namespace eng
