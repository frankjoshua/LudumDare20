PROJECT( LUDUM_DARE_20 )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

INCLUDE_DIRECTORIES( . )
INCLUDE_DIRECTORIES( /opt/local/include )
INCLUDE_DIRECTORIES( /Users/rtward/Workspace/MyGameEngine/src )

LINK_DIRECTORIES( /opt/local/lib )
LINK_DIRECTORIES( /Users/rtward/Workspace/MyGameEngine/build )

FILE( GLOB_RECURSE
	SOURCE_FILES
	*.c* )

ADD_EXECUTABLE(
    game
    ${SOURCE_FILES} )

IF( ${CMAKE_HOST_APPLE} )
    SET_PROPERTY(
            TARGET game
            APPEND PROPERTY LINK_FLAGS -Wl,-framework,Cocoa )
ENDIF( ${CMAKE_HOST_APPLE} )

TARGET_LINK_LIBRARIES(
    game
    my_game_engine
    boost_program_options
    boost_system
    SOIL
    GL
    GLU
    glut )

