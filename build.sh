cd build &&
cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . &&
ctest
cd ..

./build/X4ConverterApp/X4ConverterApp importxmf '/home/cg/Desktop/X4/unpacked' '/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_scout_01.xml' 
./build/X4ConverterApp/X4ConverterApp importxmf '/home/cg/Desktop/X4/unpacked' '/home/cg/Desktop/X4/unpacked/assets/units/size_l/ship_arg_l_miner_liquid_01.xml' 
./build/X4ConverterApp/X4ConverterApp importxmf '/home/cg/Desktop/X4/unpacked' '/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.xml' 
./build/X4ConverterApp/X4ConverterApp importxmf '/home/cg/Desktop/X4/unpacked' '/home/cg/.local/share/Steam/steamapps/common/X4 Foundations/extensions/break/assets/units/size_s/ship_gen_s_fighter_02.xml' '/home/cg/.local/share/Steam/steamapps/common/X4 Foundations/extensions' 
#./build/X4ConverterApp/X4ConverterApp exportxmf '/home/cg/Desktop/X4/unpacked' '/home/cg/Desktop/X4/unpacked/extensions/break/assets/units/size_s/ship_gen_s_fighter_02.dae' 
./build/X4ConverterApp/X4ConverterApp exportxmf '/home/cg/Desktop/X4/unpacked' '/home/cg/.local/share/Steam/steamapps/common/X4 Foundations/extensions/break/assets/units/size_s/ship_gen_s_fighter_02.out.dae' '/home/cg/.local/share/Steam/steamapps/common/X4 Foundations/extensions' 
