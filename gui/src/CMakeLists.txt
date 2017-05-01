# Define two variables in order not to repeat ourselves.
set(APPLICATION_NAME "${PROJECT_NAME}_run")


set(APP_SOURCES
    app.cpp
    main.cpp
    camera.cpp
    keyboard.cpp
    keyboardControl.cpp
    renderObject.cpp
)

set(QT_SOURCES
    mainwindow.cpp
    glwidget.cpp
)
set(QT_HEADERS
    mainwindow.h
    glwidget.h
)

set(QT_FILES
    mainwindow.ui
)

qt5_wrap_cpp(QT_HEADERS_MOC ${QT_HEADERS})
qt5_wrap_ui (QT_FILES_MOC ${QT_FILES})

include_directories ($<TARGET_PROPERTY:mapping,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:sat,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:shadowing,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:obj,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:world,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
include_directories( ${GLEW_INCLUDE_DIRS} )

# This is a make target, so you can do a "make projBrunner"
add_executable(
    ${APPLICATION_NAME} 
    ${APP_SOURCES}
    ${QT_SOURCES} 
    ${QT_HEADERS_MOC}
    ${QT_FILES_MOC} 
)

# Links the target exe (projBrunner) against the library (libprojB) and other required libraries
target_link_libraries(${APPLICATION_NAME} Qt5::Widgets ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} )
target_link_libraries(${APPLICATION_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${APPLICATION_NAME} mapping)
target_link_libraries(${APPLICATION_NAME} common)
target_link_libraries(${APPLICATION_NAME} sat)
target_link_libraries(${APPLICATION_NAME} shadowing)
target_link_libraries(${APPLICATION_NAME} obj)
target_link_libraries(${APPLICATION_NAME} world)
