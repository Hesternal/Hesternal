export module CopiumEngine.Math:Concept;

import <concepts>;


export namespace Copium
{

    template<typename T> concept CArithmetic = std::is_arithmetic_v<T>;

} // export namespace Copium
