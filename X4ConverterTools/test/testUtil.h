//
// Created by cg on 4/11/19.
//

#pragma once
#include <pugixml.hpp>
#include <set>
#include <algorithm>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <regex>
#include <cmath>
namespace test{

    class TestUtil{
    public:
        static void CompareXMLFiles(pugi::xml_document& expectedDoc, pugi::xml_document& actualDoc);

    };
}