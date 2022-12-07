//module;

//#include <fmt/format.h>

//export module CopiumEngine.Math:Format;

//import :Float3x3;
//import :Float4x4;


// NOTE(v.matushkin): Doesn't work because when I include <fmt/format.h> I get ICE


//export template<>
//struct fmt::formatter<Copium::Float4x4>
//{
//    //template<typename ParseContext>
//    //constexpr auto parse(ParseContext& ctx)
//    //{
//    //
//    //}
//
//
////    // Parses format specifications of the form ['f' | 'e'].
////    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
////    {
////        // [ctx.begin(), ctx.end()) is a character range that contains a part of
////        // the format string starting from the format specifications to be parsed,
////        // e.g. in
////        //
////        //   fmt::format("{:f} - point of interest", point{1, 2});
////        //
////        // the range will contain "f} - point of interest". The formatter should
////        // parse specifiers until '}' or the end of the range. In this example
////        // the formatter should parse the 'f' specifier and return an iterator
////        // pointing to '}'.
////
////        // Please also note that this character range may be empty, in case of
////        // the "{}" format string, so therefore you should check ctx.begin()
////        // for equality with ctx.end().
////
////        // Parse the presentation format and store it in the formatter:
////        auto it = ctx.begin(), end = ctx.end();
////        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
////
////        // Check if reached the end of the range:
////        if (it != end && *it != '}') throw format_error("invalid format");
////
////        // Return an iterator past the end of the parsed range:
////        return it;
////    }
//
//
//    // Formats the point p using the parsed format specification (presentation)
//    // stored in this formatter.
//    template <typename FormatContext>
//    auto format(const Copium::Float4x4& m, FormatContext& ctx) const -> decltype(ctx.out())
//    {
//        //// ctx.out() is an output iterator to write to.
//        //return presentation == 'f'
//        //          ? fmt::format_to(ctx.out(), "({:.1f}, {:.1f})", p.x, p.y)
//        //          : fmt::format_to(ctx.out(), "({:.1e}, {:.1e})", p.x, p.y);
//
//        return fmt::format_to(ctx.out(),
//            "(({:f}, {:f}, {:f}, {:f}), ({:f}, {:f}, {:f}, {:f}), ({:f}, {:f}, {:f}, {:f}), ({:f}, {:f}, {:f}, {:f}))",
//            m.C0.X, m.C1.X, m.C2.X, m.C3.X,
//            m.C0.Y, m.C1.Y, m.C2.Y, m.C3.Y,
//            m.C0.Z, m.C1.Z, m.C2.Z, m.C3.Z,
//            m.C0.W, m.C1.W, m.C2.W, m.C3.W
//        );
//    }
//};
//
//
//export template<>
//struct fmt::formatter<Copium::Float3x3>
//{
//    template <typename FormatContext>
//    auto format(const Copium::Float3x3& m, FormatContext& ctx) const -> decltype(ctx.out())
//    {
//        return fmt::format_to(ctx.out(),
//            "(({:f}, {:f}, {:f}), ({:f}, {:f}, {:f}), ({:f}, {:f}, {:f}))",
//            m.C0.X, m.C1.X, m.C2.X,
//            m.C0.Y, m.C1.Y, m.C2.Y,
//            m.C0.Z, m.C1.Z, m.C2.Z
//        );
//    }
//};
