export module Copium.ECS;

// NOTE(v.matushkin): I didn't want to write this modules as internal partitions,
//   but then when I `export import` them they can be imported outside of Copium.ECS lib,
//   even though I'm putting them in Private folder and I don't want that.
export import :Entity;
export import :EntityManager;
export import :IComponent;
export import :ISystem;

