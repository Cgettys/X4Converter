//BOOST_AUTO_TEST_CASE(test_read_part_offset) { // NOLINT(cert-err58-cpp)
//        const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
//        pugi::xml_document expected;
//        pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
//        BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
//        auto partNode = expected.select_node(
//        "/components/component/connections/connection[@name='Connection02']/parts/part").node();
//        BOOST_TEST_REQUIRE(!partNode.empty());
//
//        auto part = Part(partNode);
//        auto result = part.ConvertToAiNode();
//        aiMatrix4x4 expectedMatrix(aiVector3D(1,1,1), aiQuaternion(0.976296, -0.2164396, -0, -0),
//        aiVector3D(9.411734, -2.738604, -2.866085));
//
//        BOOST_TEST(result->mTransformation.Equal(expectedMatrix));
//}

// TODO animations, tags, etc
