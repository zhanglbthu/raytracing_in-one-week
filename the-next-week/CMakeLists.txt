CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(main)

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -fopenmp")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fopenmp")
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

SET(SOURCES
        main.cpp)

SET(INCLUDES
        external/stb_image.h
        color.h
        ray.h
        hittable.h
        sphere.h
        rtweekend.h
        camera.h
        interval.h
        aabb.h
        bvh.h
        texture.h
        perlin.h
        rtw_stb_image.h
        quad.h
        image.h
        vec3.h)

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${INCLUDES})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
