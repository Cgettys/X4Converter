#include "testUtil.h"

namespace test {
    std::string TestUtil::GetBasePath(){

        char* path = std::getenv("X4_UNPACKED_ROOT");
        BOOST_TEST_REQUIRE(path != nullptr);
        return std::string(path);
    }
    void TestUtil::CompareXMLFiles(pugi::xml_document &expectedDoc, pugi::xml_document &actualDoc) {

        //https://pugixml.org/docs/manual.html#access.walker
        // See also: https://stackoverflow.com/a/29599648
        struct my_walker : pugi::xml_tree_walker {
            std::set<std::string> paths;

            virtual bool for_each(pugi::xml_node &node) {
                std::vector<std::string> pathParts;
                auto parent = node;
                do {
                    std::string part = parent.name();
                    if (parent.attribute("name")) {
                        part.append("[@name='");
                        part.append(parent.attribute("name").as_string());
                        part.append("']");
                    } else if (parent.attribute("id")) {
                        part.append("[@id='");
                        part.append(parent.attribute("id").as_string());
                        part.append("']");
                    } else if (parent.attribute("index")) {
                        part.append("[@index='");
                        part.append(parent.attribute("index").as_string());
                        part.append("']");
                    }
                    pathParts.emplace(pathParts.begin(), part);
                    parent = parent.parent();
                } while (parent);
//			std::cout << path << std::endl;
                // Duplicate element == BAD
//			BOOST_TEST(!paths.count(path));
                std::string path = "/";
                for (auto &x : pathParts) {
                    path.append(x);
                    path.append("/");
                }
                path.pop_back();
                paths.insert(path);
                return true; // continue traversal
            }
        };
        my_walker expectedWalk;
        expectedDoc.traverse(expectedWalk);
        my_walker actualWalk;
        actualDoc.traverse(actualWalk);
        std::set<std::string> expectedMinusActual;
        std::set_difference(expectedWalk.paths.begin(), expectedWalk.paths.end(), actualWalk.paths.begin(),
                            actualWalk.paths.end(), std::inserter(expectedMinusActual, expectedMinusActual.begin()));

        std::set<std::string> actualMinusExpected;
        std::set_difference(actualWalk.paths.begin(), actualWalk.paths.end(), expectedWalk.paths.begin(),
                            expectedWalk.paths.end(), std::inserter(actualMinusExpected, actualMinusExpected.begin()));

        std::set<std::string> intersection;
        std::set_intersection(actualWalk.paths.begin(), actualWalk.paths.end(), expectedWalk.paths.begin(),
                              expectedWalk.paths.end(), std::inserter(intersection, intersection.begin()));
        BOOST_TEST(expectedMinusActual.size() == 0);
        for (auto &&x : expectedMinusActual) {
            printf("Output was missing path: %s\n", x.c_str());
        }
        BOOST_TEST(actualMinusExpected.size() == 0);
        for (auto &&x : actualMinusExpected) {
            printf("Output has extra path: %s\n", x.c_str());
        }


        // Slightly modified to allow integers too for ease of use: from https://stackoverflow.com/a/5578251
        auto re_float = std::regex(
                "^\\s*[+-]?([0-9]+\\.?[0-9]*([Ee][+-]?[0-9]+)?|\\.?[0-9]+([Ee][+-]?[0-9]+)?|[0-9]+[Ee][+-]?[0-9]+)$");
        for (auto &&x : intersection) {
            auto expectedNode = expectedDoc.select_node(x.c_str()).node();
            auto actualNode = actualDoc.select_node(x.c_str()).node();
            for (auto &expectedAttr: expectedNode.attributes()) {
                std::string attrName = expectedAttr.name();
                auto actualAttr = actualNode.attribute(attrName.c_str());
                BOOST_TEST(actualAttr, "Missing attribute at path: " + x + " attr name: " + attrName);
                std::string expectedValueStr = expectedAttr.as_string();
                if (!std::regex_match(expectedValueStr, re_float)) {
                    BOOST_TEST_WARN(expectedValueStr == actualAttr.as_string(),
                               "String attribute " + attrName + " at path " + x + " should be equal");
                } else {
                    // We'll assume float otherwise
                    float expectedValueFloat = expectedAttr.as_float();
                    float actualValueFloat = actualAttr.as_float();
                    const float small = 2e-6;
                    if (fabs(expectedValueFloat)>small&& fabs(actualValueFloat) > small){
                        BOOST_WARN_CLOSE(expectedValueFloat, actualValueFloat,1.0);
//                        BOOST_CHECK_CLOSE(expectedValueFloat, actualValueFloat,1.0);
                    } else {
                        BOOST_CHECK_SMALL(expectedValueFloat,small);
                        BOOST_CHECK_SMALL(actualValueFloat,small);
                    }
                }

            }

            for (auto &actualAttr: actualNode.attributes()) {
                BOOST_TEST(expectedNode.attribute(actualAttr.name()),
                           "Extra attribute at path: " + x + " attr name: " + actualAttr.name());
            }

        }

    }
};