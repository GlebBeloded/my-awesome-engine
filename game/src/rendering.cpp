#include "tetris.hpp"

#include <set>

float normalize(const float& min, const float& max,
                const float& current_value) {
    return (current_value - min) / (max - min);
}
namespace tetris {

std::pair<float, float> tile_displacement(const tile& t1, const tile& t2) {
    std::pair<float, float> t1_midpoint{
        (t1.get_data()[0].v[0].coord.x + t1.get_data()[0].v[2].coord.x) / 2,
        (t1.get_data()[0].v[0].coord.y + t1.get_data()[0].v[2].coord.y) / 2
    };
    std::pair<float, float> t2_midpoint{
        (t2.get_data()[0].v[0].coord.x + t2.get_data()[0].v[2].coord.x) / 2,
        (t2.get_data()[0].v[0].coord.y + t2.get_data()[0].v[2].coord.y) / 2
    };
    return calculate_displacement(
        matrix::vector{ t1_midpoint.first, t1_midpoint.second },
        matrix::vector{ t2_midpoint.first, t2_midpoint.second });
}

tile shrink(const tile& t) {
    auto shrinked{ t };
    shrinked.apply_matrix(matrix::scale(0.1, 0.1));
    auto displacement = tile_displacement(t, shrinked);
    shrinked.apply_matrix(
        matrix::move({ -displacement.first, -displacement.second }));
    return shrinked;
}

int get_field_index(int x, int y) {
    return x + (y * state::board_size.first);
}

std::vector<tile> lerp(const std::vector<tile>& a, const std::vector<tile>& b,
                       float alpha) {
    if (a.size() != b.size()) {
        throw std::logic_error("cannot interpolate uneven number of objects");
    }
    std::vector<tile> result(a.size());
    for (auto i = 0; i < a.size(); i++) {
        result[i] = lerp(a[i], b[i], alpha);
    }
    return result;
}
} // namespace tetris

void tetris::render_state::render(eng::engine* e, float time) {

    if (destruction_phase) {
        float alpha = normalize(d_phase_begin_time, d_phase_end_time, time);
        if (alpha > 1.0) {
            alpha              = 1.0;
            destruction_phase  = false;
            displacement_phase = true;
        }
        for (const auto& [iter, val] : full_state) {
            if (tiles_to_delete[iter])
                lerp(full_state[iter], shrink(full_state[iter]), alpha)
                    .render(e);
            else
                val.render(e);
        }
        return;
    }

    if (displacement_phase) {
        float alpha = normalize(d_phase_end_time, end_time, time);
        if (alpha > 1.0) {
            alpha    = 1.0;
            finished = true;
        }
        for (const auto& [key, value] : full_state) {

            if (tiles_to_delete[key])
                continue;
            if (key < threshhold_value)
                value.render(e);
            if (key >= threshhold_value) {
                auto moved{ value };
                moved.move_to_coords(value.coords.first,
                                     value.coords.second - rows_destroyed);
                lerp(value, moved, alpha).render(e);
            }
        }
        return;
    } else
        for (const auto& [key, value] : full_state)
            value.render(e);
}

void tetris::render_state::initialize(float b, float e) {
    if (initialized == true)
        throw std::logic_error("render process already taking place");

    d_phase_begin_time = b;
    d_phase_end_time   = e - ((e - b) / 2.f);
    end_time           = e;
    initialized        = true;
    finished           = false;
    destruction_phase  = true;
}

void tetris::render_state::reset() {
    initialized = false;
    for (const auto& [key, val] : tiles_to_delete) {
        if (val)
            full_state.erase(key);
    }
    std::set<int> invalid_indexes;
    // move all tyles n rows lower
    for (auto [key, value] : full_state) {
        if (key >= threshhold_value) {
            value.move_to_coords(value.coords.first,
                                 value.coords.second - rows_destroyed);
            auto index =
                get_field_index(value.coords.first, value.coords.second);
            full_state[index] = value;
            invalid_indexes.emplace(key);
            if (invalid_indexes.find(index) != invalid_indexes.end()) {
                invalid_indexes.erase(index);
            }
        }
    }

    for (const auto& index : invalid_indexes) {
        full_state.erase(index);
    }

    threshhold_value = state::board_size.first * (state::board_size.second + 5);
    finished         = false;
    rows_destroyed   = 0;
    destruction_phase  = false;
    displacement_phase = false;

    tiles_to_delete.clear();
    adjustment = 0;
}
