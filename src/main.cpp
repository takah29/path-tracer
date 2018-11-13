#include <map>
#include <string>
#include <vector>
#include "build.hpp"
#include "scene.hpp"

std::map<std::string, std::string> get_params(std::vector<std::string>& args) {
    // Default parameter
    std::map<std::string, std::string> params{
        {"samples", "2"},      {"super_samples", "4"}, {"plane_width", "1.5"}, {"width_res", "320"},
        {"height_res", "240"}, {"scene_num", "1"},     {"tracer", "pt"}};

    for (size_t i = 1; i < args.size(); i += 2) {
        if (args[i] == "-s") {
            params["samples"] = args[i + 1];
        } else if (args[i] == "-ss") {
            params["super_samples"] = args[i + 1];
        } else if (args[i] == "-w") {
            params["width_res"] = args[i + 1];
        } else if (args[i] == "-h") {
            params["height_res"] = args[i + 1];
        } else if (args[i] == "-n") {
            params["scene_num"] = args[i + 1];
        }
    }

    printf("========== Parameter ==========\n");
    printf("Scene number : %s\n", params["scene_num"].c_str());
    // printf("Tracer       : %s\n", params["tracer"].c_str());
    printf("Samples      : %s\n", params["samples"].c_str());
    printf("Super-Samples: %s\n", params["super_samples"].c_str());
    printf("Resolution   : %sx%s\n", params["width_res"].c_str(), params["height_res"].c_str());
    printf("===============================\n");

    return params;
}

int main(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);
    if (argc % 2 != 1) {
        printf("Error: Wrong number of arguments.\n");
        return 1;
    }

    auto params = get_params(args);

    Scene scene;
    switch (std::stoi(params["scene_num"])) {
        case 1: {
            if (!build_1(scene)) return 1;
            break;
        };
        case 2: {
            if (!build_2(scene)) return 1;
            break;
        };
        case 3: {
            if (!build_3(scene)) return 1;
            break;
        };
        case 4: {
            if (!build_4(scene)) return 1;
            break;
        };
        case 5: {
            if (!build_5(scene)) return 1;
            break;
        };

        default: {
            if (!build_1(scene)) return 1;
            break;
        }
    }

    scene.render(params);

    return 0;
}
