# CMakeList.txt : CMake project for raygbiv_cpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(Threads)

# Add source to this project's executable.
add_executable (raygbiv_cpp "raygbiv_cpp.cpp" "raygbiv_cpp.h" "stb_image_write.h" "vec3.h" "color.h" "ray.h" "hittable.h" "sphere.h" "hittable_list.h" "rtweekend.h" "camera.h" "material.h" "moving_sphere.h" "aabb.h" "bvh_node.h" "texture.h" "perlin.h" "rtw_stb_image.h" "stb_image.h" "aarect.h" "box.h" "constant_medium.h" "threadpool.h" "onb.h" "pdf.h" "scene.cpp" "scene.h" "hittable.cpp" "hittable_list.cpp" "aabb.cpp" "sphere.cpp" "onb.cpp" "aarect.cpp" "image_buffer.h" "image_buffer.cpp")

target_link_libraries(raygbiv_cpp Threads::Threads)

add_executable (mctest "montecarlo.cpp" "montecarlo.h" "stb_image_write.h" "vec3.h" "color.h" "ray.h" "hittable.h" "sphere.h" "hittable_list.h" "rtweekend.h" "camera.h" "material.h" "moving_sphere.h" "aabb.h" "bvh_node.h" "texture.h" "perlin.h" "rtw_stb_image.h" "stb_image.h" "aarect.h" "box.h" "constant_medium.h" "threadpool.h" "onb.h" "pdf.h" "hittable.cpp" "hittable_list.cpp" "aabb.cpp" "sphere.cpp" "onb.cpp" "aarect.cpp" "image_buffer.h" "image_buffer.cpp")

# TODO: Add tests and install targets if needed.
