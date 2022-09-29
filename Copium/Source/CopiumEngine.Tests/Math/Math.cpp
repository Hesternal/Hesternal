import CopiumEngine.Math;

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace Catch;
using namespace Copium;


TEST_CASE("Math::Float3", "[Math]")
{
    SECTION("constructor scalar")
    {
        Float3 a(-1.5f);

        CHECK(a.X == Approx(-1.5f));
        CHECK(a.Y == Approx(-1.5f));
        CHECK(a.Z == Approx(-1.5f));
    }
    SECTION("constructor")
    {
        Float3 a(-1.1f, 2.2f, -3.3f);

        CHECK(a.X == Approx(-1.1f));
        CHECK(a.Y == Approx(2.2f));
        CHECK(a.Z == Approx(-3.3f));
    }

    SECTION("add")
    {
        Float3 a(-1.1f, 2.2f, -3.3f);
        Float3 b(-1.1f, 2.2f, -3.3f);
        Float3 c(-2.2f, 4.4f, -6.6f);

        Float3 r = a + b;

        CHECK(r.X == Approx(c.X));
        CHECK(r.Y == Approx(c.Y));
        CHECK(r.Z == Approx(c.Z));
    }
}
