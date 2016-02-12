project(lwip_arch)


set( INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/proj
        ${CMAKE_CURRENT_SOURCE_DIR}/netif
        )
        

set( SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/sys_arch.c
        ${CMAKE_CURRENT_SOURCE_DIR}/proj/lwipconfig.c
        ${CMAKE_CURRENT_SOURCE_DIR}/netif/rpmsgEthernet.c )
        
set (LWIP_ARCH_SRC ${SRC}  PARENT_SCOPE)