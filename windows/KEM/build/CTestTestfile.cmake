# CMake generated Testfile for 
# Source directory: C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM
# Build directory: C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TestKeyImages "C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/build/test_key_images.exe")
set_tests_properties(TestKeyImages PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/CMakeLists.txt;178;add_test;C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/CMakeLists.txt;0;")
add_test(BenchmarkTest "C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/build/benchmark_color_kem_timing.exe")
set_tests_properties(BenchmarkTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/CMakeLists.txt;196;add_test;C:/Users/janga/TRENDYWALLPAPER/ColorKEM/windows/KEM/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
subdirs("tests")
