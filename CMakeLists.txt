# =============================================================================
# Analisador B3 - prototipo de telas (entrega do 1o bimestre)
# Trabalho de Analise e Projeto de Sistemas (UTFPR)
#
# Alvos:
#   analisador-b3-prototipo  (executavel) - prototipo de telas: interface grafica
#                                           (src/ui) e dominio (src/dominio), com
#                                           dados fixos em memoria (src/prototipo);
#                                           sem SQL nem rede
#   capturar-telas-prototipo (executavel) - salva um PNG de cada tela (modo
#                                           offscreen)
#
# Requisitos: CMake 3.19+, compilador C++20 e Qt 6 (Core, Gui, Widgets, Charts).
#
# Compilar:
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#   ./build/analisador-b3-prototipo
# =============================================================================

cmake_minimum_required(VERSION 3.19)

project(AnalisadorB3
    VERSION 1.0.0
    DESCRIPTION "Analise de ativos da B3 com recomendacoes explicadas por regras"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt exige geracao automatica de codigo para signals/slots (moc) e recursos (rcc).
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de build" FORCE)
endif()

option(APENAS_PROTOTIPO "Compila somente o prototipo de telas (sem SQL, rede nem testes)" ON)

if(NOT APENAS_PROTOTIPO)
    message(FATAL_ERROR "Esta versao contem somente o prototipo de telas: mantenha APENAS_PROTOTIPO=ON.")
endif()
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)

# Fontes da interface grafica, compartilhadas pelo sistema completo e pelo prototipo.
file(GLOB_RECURSE FONTES_INTERFACE CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp")

# -----------------------------------------------------------------------------
# Prototipo de telas (entrega de 13/10, 1o bimestre).
# Compila as mesmas telas de src/ui e o dominio, mas NENHUM .cpp de analise/,
# persistencia/ ou servicos/: os stubs de src/prototipo implementam as mesmas
# classes (mesmos headers) sobre dados fixos em memoria. Por isso nao linka
# Qt6::Sql nem Qt6::Network. A definicao ANALISADOR_PROTOTIPO liga a marcacao
# "(prototipo)" na janela principal e o modo sem banco de capturar-telas.
# -----------------------------------------------------------------------------
file(GLOB FONTES_DOMINIO CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dominio/*.cpp")
file(GLOB FONTES_PROTOTIPO CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/prototipo/*.cpp")
# main.cpp do prototipo fica fora da lista comum: capturar-telas-prototipo tem o seu.
list(REMOVE_ITEM FONTES_PROTOTIPO "${CMAKE_CURRENT_SOURCE_DIR}/src/prototipo/main.cpp")
set(RECURSOS_PROTOTIPO "${CMAKE_CURRENT_SOURCE_DIR}/recursos/prototipo.qrc")

add_executable(analisador-b3-prototipo
    "${CMAKE_CURRENT_SOURCE_DIR}/src/prototipo/main.cpp"
    ${FONTES_PROTOTIPO}
    ${FONTES_DOMINIO}
    ${FONTES_INTERFACE}
    ${RECURSOS_PROTOTIPO})

add_executable(capturar-telas-prototipo
    "${CMAKE_CURRENT_SOURCE_DIR}/ferramentas/capturar-telas.cpp"
    ${FONTES_PROTOTIPO}
    ${FONTES_DOMINIO}
    ${FONTES_INTERFACE}
    ${RECURSOS_PROTOTIPO})

foreach(ALVO_PROTOTIPO analisador-b3-prototipo capturar-telas-prototipo)
    target_compile_definitions(${ALVO_PROTOTIPO} PRIVATE ANALISADOR_PROTOTIPO)
    target_include_directories(${ALVO_PROTOTIPO} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(${ALVO_PROTOTIPO} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Charts)
endforeach()

message(STATUS "AnalisadorB3 ${PROJECT_VERSION} - somente prototipo de telas - Qt ${Qt6_VERSION} - build ${CMAKE_BUILD_TYPE}")
