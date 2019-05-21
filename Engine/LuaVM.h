#ifndef LUA_VM
#define LUA_VM

#include <string>
#include <iostream>

extern "C"{
	#include <lua/lua.h>
	#include <lua/lauxlib.h>
	#include <lua/lualib.h>
}

namespace script {

	template<class T> 
	class LuaVM {
	public:
		typedef int (T::*mem_func)(lua_State * L);

		LuaVM(){
			L = luaL_newstate();
			luaL_openlibs(L);
		}

		~LuaVM(){
			lua_close(L);
		}

		void registerInstance(T* instance){
			*static_cast<T**>(lua_getextraspace(L)) = instance;
		}

		void registerMethod(const std::string& name, int(*func)(lua_State*)){
			lua_register(L, name.c_str(), func);
		}

		void doString(const std::string& script){
			if(checkStatus(luaL_dostring(L, script.c_str()))){
				std::cout << "script rulat cu succes" << std::endl;
			}
		}

		// This template wraps a member function into a C-style "free" function compatible with lua.
		template <mem_func func>
		static int dispatch(lua_State* L) {
			T* ptr = *static_cast<T**>(lua_getextraspace(L));
			return ((*ptr).*func)(L);
		}

	private:
		bool checkStatus(int res){
			if(res != LUA_OK){
				std::string err = lua_tostring(L, -1);
				std::cout << err << std::endl;
				return false;
			}
			return true;
		}


		lua_State* L;
	};

}
#endif //LUA_VM
