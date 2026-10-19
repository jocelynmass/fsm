 #.
 # Copyright (c) 2017-2019 Jocelyn Masserot.  All rights reserved.
 #.
 # Permission is hereby granted, free of charge, to any person obtaining a copy
 # of this software and associated documentation files (the "Software"), to
 # deal with the Software without restriction, including without limitation the
 # rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 # sell copies of the Software, and to permit persons to whom the Software is
 # furnished to do so, subject to the following conditions:
 #.
 #  1. Redistributions of source code must retain the above copyright notice,
 #     this list of conditions and the following disclaimers.
 #  2. Redistributions in binary form must reproduce the above copyright
 #     notice, this list of conditions and the following disclaimers in the
 #     documentation and/or other materials provided with the distribution.
 #  3. Neither the name of Jocelyn Masserot, nor the names of its contributors
 #     may be used to endorse or promote products derived from this Software
 #     without specific prior written permission.
 #.
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 # CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 # FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 # WITH THE SOFTWARE.
 #.

target_compile_definitions(ecko-lib
	INTERFACE
		USE_LIB_FSM=1
)

set(LIB_INC ${LIB_INC} "${CMAKE_CURRENT_LIST_DIR}/includes")
set(LIB_INC ${LIB_INC} "${CMAKE_CURRENT_LIST_DIR}/port")
set(LIB_SRC ${LIB_SRC} "${CMAKE_CURRENT_LIST_DIR}/src/fsm.c")

if(FSM_FREERTOS_PORT)
    target_compile_definitions(ecko-lib
        INTERFACE
        FSM_FREERTOS_PORT=1
    )
endif()

target_include_directories(ecko-lib
    INTERFACE
        ${LIB_INC}
)

target_sources(ecko-lib
    INTERFACE
        ${LIB_SRC}
)