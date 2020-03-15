/**
 * @file interface_traits.h
 * Addle source code
 * 
 * @copyright
 * Copyright 2019 Eleanor Hawk
 * 
 * @copyright
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#ifndef INITIALIZE_TRAITS
#define INITIALIZE_TRAITS

#include <type_traits>

/**
 * @struct expects_initialize
 * @brief A static (compile-time) trait describing whether an interface expects
 * to be initailized
 * 
 * Use DECL_EXPECTS_INITIALIZE to apply this trait to an interface.
 *  
 * @sa
 * - [Initialization](src/docs/initialization.md)
 * - InitializeHelper
 * - DECL_EXPECTS_INITIALIZE
 */
template<class T>
struct expects_initialize : std::false_type
{
};

/**
 * @def DECL_EXPECTS_INITIALIZE
 * @brief
 * Convenience macro declaring that `Interface` has trait expects_initialize.
 * 
 * @param Interface
 * The interface being declared with the trait
 */
#define DECL_EXPECTS_INITIALIZE(Interface) template<> struct expects_initialize<Interface> : std::true_type {};

#endif //INITIALIZE_TRAITS