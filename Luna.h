#pragma once
#ifndef __luna_h__
#define __luna_h__

#include "lua.hpp"

//-----------------------------------------------------------------------------

#define luaL_assert(L, cond, msg) if (!(cond)) { return luaL_error(L, (msg)); }

// functions and properties declarators
#define LUNA_DECL(methodname)              int luna_##methodname(lua_State *L)
#define LUNA_DECLEX(subset, methodname)    int subset::luna_##methodname(lua_State *L)
#define LUNA_GETTER(methodname)            int luna_get##methodname(lua_State *L)
#define LUNA_GETTEREX(subset, methodname)  int subset::luna_get##methodname(lua_State *L)
#define LUNA_SETTER(methodname)            int luna_set##methodname(lua_State *L)
#define LUNA_SETTEREX(subset, methodname)  int subset::luna_set##methodname(lua_State *L)
// response table entries
#define LUNA_FUNC(funcname)                {#funcname, &luna_##funcname}
#define LUNA_FUNCEX(subset, funcname)      {#funcname, &subset::luna_##funcname}
#define LUNA_METHOD(methodname)            {#methodname, &luna_##methodname, &luna_##methodname}
#define LUNA_METHODEX(subset, methodname)  {#methodname, &subset::luna_##methodname, &subset::luna_##methodname}
#define LUNA_PROP(methodname)              {#methodname, &luna_get##methodname, &luna_set##methodname}
#define LUNA_PROPEX(subset, methodname)    {#methodname, &subset::luna_get##methodname, &subset::luna_set##methodname}
#define LUNA_RPROP(methodname)             {#methodname, &luna_get##methodname, nullptr}
#define LUNA_RPROPEX(subset, methodname)   {#methodname, &subset::luna_get##methodname, nullptr}
#define LUNA_WPROP(methodname)             {#methodname, nullptr, &luna_set##methodname}
#define LUNA_WPROPEX(subset, methodname)   {#methodname, nullptr, &subset::luna_set##methodname}

// Luna class declaration
#define LUNAPTR_CLASS(T) \
	typedef LunaPtr<T> Luna; \
	typedef typename Luna::PropertyType PropertyType; \
	typedef typename Luna::FunctionType FunctionType; \
	static const char classname[]; \
	static const PropertyType properties[]; \
	static const FunctionType methods[];
#define LUNAOBJ_CLASS(T) \
	typedef LunaObj<T> Luna; \
	typedef typename Luna::PropertyType PropertyType; \
	typedef typename Luna::FunctionType FunctionType; \
	static const char classname[]; \
	static const PropertyType properties[]; \
	static const FunctionType methods[];

//-----------------------------------------------------------------------------

template <typename T> class LunaPtr {
public:

	typedef struct {
		const char* name;
		int         (T::*getter) (lua_State *);
		int         (T::*setter) (lua_State *);
	} PropertyType;

	typedef struct {
		const char*    name;
		lua_CFunction  method;
	} FunctionType;

	typedef struct {
		std::shared_ptr<T> object;
	} userdataType;

	static void Register(lua_State* L) {
		luaL_newmetatable(L, T::classname);
		int metatable = lua_gettop(L);

		// store method table in globals so that
		// scripts can add functions written in Lua.
		lua_pushvalue(L, -1);
		lua_setglobal(L, T::classname);

		lua_pushliteral(L, "new");
		lua_pushcfunction(L, constructor);
		lua_settable(L, metatable); // add default constructor to method table

		lua_pushliteral(L, "is");
		lua_pushcfunction(L, is);
		lua_settable(L, metatable); // add type check to method table

		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, gc_instance);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__tostring");
		lua_pushcfunction(L, tostring);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__eq");
		lua_pushcfunction(L, equals);
		lua_settable(L, metatable); // to be able to compare two Luna objects (not natively possible with full userdata)

		lua_pushliteral(L, "__index");
		lua_pushcfunction(L, property_getter);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__newindex");
		lua_pushcfunction(L, property_setter);
		lua_settable(L, metatable);

		for (int i = 0; T::properties[i].name; i++) { // Register some functions/properties in it
			lua_pushstring(L, T::properties[i].name); // Having some string associated with them
			lua_pushnumber(L, i);                     // And a number indexing which function/property it is
			lua_settable(L, metatable);
		}

		// fill method table with methods from class T
		for (const FunctionType* l = T::methods; l->name; l++) {
			lua_pushstring(L, l->name);
			lua_pushcfunction(L, l->method);
			lua_settable(L, metatable);
		}

		lua_pop(L, 1); // drop metatable
	}

	// create a new T object and
	// push onto the Lua stack a userdata containing a pointer to T object
	static int constructor(lua_State* L) {
		auto instance = std::make_shared<T>(); // let's call constructor
		push(L, instance);
		return 1; // userdata containing pointer to T object
	}

	static int is(lua_State* L) {
		auto ret = opt(L, 1);
		lua_pushboolean(L, ret != nullptr);
		return 1;
	}

	static int tostring(lua_State *L) {
		userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
		if (ud) {
			__if_exists(T::tostring) {
				ud->object->tostring(L);
			}
			__if_not_exists(T::tostring) {
				lua_pushfstring(L, "%s (%p)", T::classname, ud->object.get());
			}
		} else {
			lua_pushliteral(L, "empty object");
		}
		return 1;
	}

	static int equals(lua_State *L) {
		auto ptr1 = opt(L, 1);
		auto ptr2 = opt(L, 2);
		lua_pushboolean(L, ptr1 == ptr2);
		return 1;
	}

	static void push(lua_State* L, std::shared_ptr<T> instance) {
		userdataType* ud = static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
		new (ud) userdataType();
		ud->object = instance;
		luaL_getmetatable(L, T::classname); // lookup metatable in Lua registry
		lua_setmetatable(L, -2);
	}

	// get userdata from Lua stack and return pointer to T object
	static std::shared_ptr<T> check(lua_State *L, int narg) {
		userdataType *ud = static_cast<userdataType*>(luaL_checkudata(L, narg, T::classname));
		return ud->object; // pointer to T object
	}

	static std::shared_ptr<T> opt(lua_State *L, int narg) {
		std::shared_ptr<T> ret = nullptr;
		userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, narg));
		if (ud) { // value is a userdata?
			if (lua_getmetatable(L, narg)) { // does it have a metatable?
				lua_getfield(L, LUA_REGISTRYINDEX, T::classname); // get correct metatable
				if (lua_rawequal(L, -1, -2)) { // does it have the correct mt?
					ret = ud->object;
				}
				lua_pop(L, 2); // remove both metatables
			}
		}
		return ret;
	}

private:
	LunaPtr() {} // hide default constructor

	static int gc_instance(lua_State* L) { // disable direct call to garbage collector method
		userdataType* ud = static_cast<userdataType*>(lua_touserdata(L, 1));
		__if_exists(T::gc_reset) {
			ud->object->gc_reset();
		}
		ud->~userdataType();
		return 0;
	}

	static int property_getter(lua_State * L) {
		lua_getmetatable(L, 1); // Look up the index of a name
		lua_pushvalue(L, 2);    // Push the name
		lua_rawget(L, -2);      // Get the index

		if (lua_isnumber(L, -1)) { // Check if we got a valid index
			size_t index = (size_t)lua_tointeger(L, -1);
			const PropertyType* l = &T::properties[index];
			if (!l->getter) {
				return luaL_error( L , "no getter \"%s\" of class \"%s\" defined", l->name, T::classname );
			}

			userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
#ifdef _DEBUG
			if (!ud) {
				return luaL_error( L , "internal error, no object given!" );
			}
#endif
			T* self = ud->object.get();

			if (l->getter == l->setter) { // Call function
				lua_pushinteger(L, index);
				lua_pushlightuserdata(L, self);
				lua_pushcclosure(L, dispatch, 2);
				return 1; // Return a func
			} else { // Call property getter
				lua_pop(L, 2);    // Pop metatable and index
				lua_remove(L, 1); // Remove userdata
				lua_remove(L, 1); // Remove [key]
				return (self->*(T::properties[index].getter))(L);
			}
		} else if (lua_isnil(L, -1)) {
			if (lua_getmetatable(L, -2)) { // Give opportunity to call user defined metamethods
				lua_pushvalue(L, 2);
				lua_gettable(L, -2);
			}
			return 1; // Return inherited function
		}

		return 1;
	}

	static int property_setter(lua_State * L) {
		lua_getmetatable(L, 1); // Look up the index of a name
		lua_pushvalue(L, 2);    // Push the name
		lua_rawget(L, -2);      // Get the index

		if (lua_isnumber(L, -1)) { // Check if we got a valid index
			size_t index = (size_t)lua_tointeger(L, -1);
			const PropertyType* l = &T::properties[index];
			if (!l->setter) {
				return luaL_error( L , "no setter \"%s\" of class \"%s\" defined", l->name, T::classname );
			} else if (l->setter == l->getter) {
				return luaL_error( L , "trying to set the method \"%s\" of class \"%s\"", l->name, T::classname );
			}

			userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
#ifdef _DEBUG
			if (!ud) {
				return luaL_error( L , "internal error, no object given!" );
			}
#endif
			T* self = ud->object.get();

			lua_pop(L, 2);    // Pop metatable and index
			lua_remove(L, 1); // Remove userdata
			lua_remove(L, 1); // Remove [key]

			return (self->*(l->setter))(L); // call member function
		}

		return 1;
	}

	static int dispatch(lua_State * L)
	{
		// get index in properties table
		size_t i = (size_t)lua_tointeger(L, lua_upvalueindex(1));
		T* self = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(2)));
		lua_remove(L, 1); // remove self so member function args start at index 1
		return (self->*(T::properties[i].getter))(L);
	}
};

//-----------------------------------------------------------------------------

template <typename T> class LunaObj {
public:

	typedef struct {
		const char* name;
		int         (T::*getter) (lua_State *);
		int         (T::*setter) (lua_State *);
	} PropertyType;

	typedef struct {
		const char*    name;
		lua_CFunction  method;
	} FunctionType;

	static void Register(lua_State* L) {
		luaL_newmetatable(L, T::classname);
		int metatable = lua_gettop(L);

		// store method table in globals so that
		// scripts can add functions written in Lua.
		lua_pushvalue(L, -1);
		lua_setglobal(L, T::classname);

		lua_pushliteral(L, "new");
		lua_pushcfunction(L, constructor);
		lua_settable(L, metatable); // add default constructor to method table

		lua_pushliteral(L, "is");
		lua_pushcfunction(L, is);
		lua_settable(L, metatable); // add type check to method table

		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, gc_instance);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__tostring");
		lua_pushcfunction(L, tostring);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__eq");
		lua_pushcfunction(L, equals);
		lua_settable(L, metatable); // to be able to compare two Luna objects (not natively possible with full userdata)

		lua_pushliteral(L, "__index");
		lua_pushcfunction(L, property_getter);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__newindex");
		lua_pushcfunction(L, property_setter);
		lua_settable(L, metatable);

		for (int i = 0; T::properties[i].name; i++) { // Register some functions/properties in it
			lua_pushstring(L, T::properties[i].name); // Having some string associated with them
			lua_pushnumber(L, i);                     // And a number indexing which function/property it is
			lua_settable(L, metatable);
		}

		// fill method table with methods from class T
		for (const FunctionType* l = T::methods; l->name; l++) {
			lua_pushstring(L, l->name);
			lua_pushcfunction(L, l->method);
			lua_settable(L, metatable);
		}

		lua_pop(L, 1); // drop metatable
	}

	// create a new T object and
	// push onto the Lua stack a userdata containing a pointer to T object
	static int constructor(lua_State* L) {
		push(L, T());
		return 1;
	}

	static int is(lua_State* L) {
		auto ret = opt(L, 1);
		lua_pushboolean(L, ret != nullptr);
		return 1;
	}

	static int tostring(lua_State *L) {
		T *ud = static_cast<T*>(lua_touserdata(L, 1));
		if (ud) {
			__if_exists(T::tostring) {
				ud->tostring(L);
			}
			__if_not_exists(T::tostring) {
				lua_pushfstring(L, "%s (%p)", T::classname, ud);
			}
		} else {
			lua_pushliteral(L, "empty object");
		}
		return 1;
	}

	static int equals(lua_State *L) {
		auto ptr1 = opt(L, 1);
		auto ptr2 = opt(L, 2);
		lua_pushboolean(L, ptr1 && ptr2 && *ptr1 == *ptr2);
		return 1;
	}

	static void push(lua_State* L, const T& instance) {
		T* ud = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
		new (ud) T(instance);
		luaL_getmetatable(L, T::classname); // lookup metatable in Lua registry
		lua_setmetatable(L, -2);
	}

	static void push(lua_State* L, const T&& instance) {
		T* ud = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
		new (ud) T(instance);
		luaL_getmetatable(L, T::classname); // lookup metatable in Lua registry
		lua_setmetatable(L, -2);
	}

	// get userdata from Lua stack and return pointer to T object
	static T *check(lua_State *L, int narg) {
		return static_cast<T*>(luaL_checkudata(L, narg, T::classname));
	}

	static T *opt(lua_State *L, int narg) {
		T *ret = nullptr;
		T *p = static_cast<T*>(lua_touserdata(L, narg));
		if (p) { // value is a userdata?
			if (lua_getmetatable(L, narg)) { // does it have a metatable?
				lua_getfield(L, LUA_REGISTRYINDEX, T::classname); // get correct metatable
				if (lua_rawequal(L, -1, -2)) { // does it have the correct mt?
					ret = p;
				}
				lua_pop(L, 2); // remove both metatables
			}
		}
		return ret;
	}

private:
	LunaObj() {} // hide default constructor

	static int gc_instance(lua_State* L) { // disable direct call to garbage collector method
		T* ud = static_cast<T*>(lua_touserdata(L, 1));
		if (ud) {
			__if_exists(T::gc_reset) {
				ud->gc_reset();
			}
			ud->~T();
		}
		return 0;
	}

	static int property_getter(lua_State * L) {
		lua_getmetatable(L, 1); // Look up the index of a name
		lua_pushvalue(L, 2);    // Push the name
		lua_rawget(L, -2);      // Get the index

		if (lua_isnumber(L, -1)) { // Check if we got a valid index
			size_t index = (size_t)lua_tointeger(L, -1);
			const PropertyType* l = &T::properties[index];
			if (!l->getter) {
				return luaL_error( L , "no getter \"%s\" of class \"%s\" defined", l->name, T::classname );
			}

			T* self = static_cast<T*>(lua_touserdata(L, 1));
#ifdef _DEBUG
			if (!self) {
				return luaL_error( L , "internal error, no object given!" );
			}
#endif

			if (l->getter == l->setter) { // Call function
				lua_pushinteger(L, index);
				lua_pushlightuserdata(L, self);
				lua_pushcclosure(L, dispatch, 2);
				return 1; // Return a func
			} else { // Call property getter
				lua_pop(L, 2);    // Pop metatable and index
				lua_remove(L, 1); // Remove userdata
				lua_remove(L, 1); // Remove [key]
				return (self->*(T::properties[index].getter))(L);
			}
		} else if (lua_isnil(L, -1)) {
			if (lua_getmetatable(L, -2)) { // Give opportunity to call user defined metamethods
				lua_pushvalue(L, 2);
				lua_gettable(L, -2);
			}
			return 1; // Return inherited function
		}

		return 1;
	}

	static int property_setter(lua_State * L) {
		lua_getmetatable(L, 1); // Look up the index of a name
		lua_pushvalue(L, 2);    // Push the name
		lua_rawget(L, -2);      // Get the index

		if (lua_isnumber(L, -1)) { // Check if we got a valid index
			size_t index = (size_t)lua_tointeger(L, -1);
			const PropertyType* l = &T::properties[index];
			if (!l->setter) {
				return luaL_error( L , "no setter \"%s\" of class \"%s\" defined", l->name, T::classname );
			} else if (l->setter == l->getter) {
				return luaL_error( L , "trying to set the method \"%s\" of class \"%s\"", l->name, T::classname );
			}

			T* self = static_cast<T*>(lua_touserdata(L, 1));
#ifdef _DEBUG
			if (!self) {
				return luaL_error( L , "internal error, no object given!" );
			}
#endif

			lua_pop(L, 2);    // Pop metatable and index
			lua_remove(L, 1); // Remove userdata
			lua_remove(L, 1); // Remove [key]

			return (self->*(l->setter))(L); // call member function
		}

		return 1;
	}

	static int dispatch(lua_State * L)
	{
		// get index in properties table
		size_t i = (size_t)lua_tointeger(L, lua_upvalueindex(1));
		T* self = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(2)));
		//T* self = static_cast<T*>(lua_touserdata(L, 1);
		lua_remove(L, 1); // remove self so member function args start at index 1
		return (self->*(T::properties[i].getter))(L);
	}
};

//-----------------------------------------------------------------------------

#endif // __luna_h__