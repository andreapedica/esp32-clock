set(COMPONENT_SRCS main.c Holtek/Holtek.c WiFiConn/WiFiConn.c )
set(COMPONENT_ADD_INCLUDEDIRS " " "./"  "./Holtek" "./WiFiConn" )

register_component()
