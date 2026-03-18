set -e

CXX=g++
CXXFLAGS="-std=c++20 -Wall -Wextra"
CPP_FILES=$(find src -name "*.cpp")
PKGS="sdl3 sdl3-image"

$CXX $CPP_FILES -o game $CXXFLAGS $(pkg-config --cflags --libs $PKGS)