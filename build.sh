#!/usr/bin/env bash
cd cmake-build-debug &&
cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . -- -j16 &&
#ctest
#./X4ConverterTools/test/test_executable
cd ..

UNPACKED_DIR="$HOME/Desktop/X4/unpacked"
#./build/X4ConverterApp/X4ConverterApp importxmf "${UNPACKED_DIR}" "${UNPACKED_DIR}/assets/units/size_s/ship_arg_s_scout_01.xml"
#./build/X4ConverterApp/X4ConverterApp importxmf "${UNPACKED_DIR}" "${UNPACKED_DIR}/assets/units/size_l/ship_arg_l_miner_liquid_01.xml" 
#./build/X4ConverterApp/X4ConverterApp importxmf "${UNPACKED_DIR}" "${UNPACKED_DIR}/assets/units/size_s/ship_gen_s_fighter_01.xml" 
#./build/X4ConverterApp/X4ConverterApp exportxmf "${UNPACKED_DIR}""${UNPACKED_DIR}/ship_gen_s_fighter_02.dae" 

#./build/X4ConverterApp/X4ConverterApp importxmf "${UNPACKED_DIR}" "${UNPACKED_DIR}/extensions/break/assets/units/size_s/ship_gen_s_fighter_02.xml" 
#./build/X4ConverterApp/X4ConverterApp exportxmf "${UNPACKED_DIR}" "${UNPACKED_DIR}/extensions/break/assets/units/size_s/ship_gen_s_fighter_02.out.dae"
