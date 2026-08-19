# =============================================================================
# Analisador B3 - sistema de analise e apoio a decisao de investimentos na B3
# Trabalho de Analise e Projeto de Sistemas (UTFPR)
#
# Estrutura de alvos:
#   analisador-nucleo  (biblioteca estatica) - dominio + analise + persistencia + servicos
#   analisador-b3      (executavel)          - nucleo + interface grafica (Qt Widgets/Charts)
#   teste-*            (testes Qt Test)      - nucleo + arquivo de teste
#
# Compilar:
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#   ./build/analisador-b3
#   ctest --test-dir build --output-on-failure
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

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts Sql Test)

# -----------------------------------------------------------------------------
# Biblioteca de nucleo: tudo que nao depende da interface grafica.
# O GLOB com CONFIGURE_DEPENDS faz o CMake reconfigurar sozinho quando um
# arquivo novo aparece - conveniente enquanto o projeto esta sendo escrito.
# -----------------------------------------------------------------------------
file(GLOB_RECURSE FONTES_NUCLEO CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dominio/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/analise/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/persistencia/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/servicos/*.cpp")

# O .qrc NAO entra na biblioteca estatica: o registro do recurso depende de um
# inicializador estatico que o linker descarta em bibliotecas. Por isso o
# recurso e compilado em cada alvo executavel (aplicacao e testes).
set(ARQUIVO_RECURSOS "${CMAKE_CURRENT_SOURCE_DIR}/recursos/recursos.qrc")

add_library(analisador-nucleo STATIC ${FONTES_NUCLEO})

target_include_directories(analisador-nucleo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(analisador-nucleo PUBLIC Qt6::Core Qt6::Sql)

# -----------------------------------------------------------------------------
# Aplicacao com interface grafica.
# -----------------------------------------------------------------------------
file(GLOB_RECURSE FONTES_INTERFACE CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp")

add_executable(analisador-b3
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    ${FONTES_INTERFACE}
    ${ARQUIVO_RECURSOS})

target_link_libraries(analisador-b3 PRIVATE
    analisador-nucleo
    Qt6::Widgets
    Qt6::Charts)

# -----------------------------------------------------------------------------
# Utilitario de captura de telas: renderiza cada tela e salva PNG (funciona com
# a plataforma "offscreen"). Serve de verificacao visual automatizada e gera as
# figuras do capitulo "Prototipo" do documento da disciplina.
# -----------------------------------------------------------------------------
add_executable(capturar-telas
    "${CMAKE_CURRENT_SOURCE_DIR}/ferramentas/capturar-telas.cpp"
    ${FONTES_INTERFACE}
    ${ARQUIVO_RECURSOS})

target_link_libraries(capturar-telas PRIVATE
    analisador-nucleo
    Qt6::Widgets
    Qt6::Charts)

# -----------------------------------------------------------------------------
# Testes (Qt Test). Cada arquivo em testes/ vira um alvo e um caso do ctest.
# -----------------------------------------------------------------------------
enable_testing()

file(GLOB ARQUIVOS_DE_TESTE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/testes/*.cpp")

foreach(ARQUIVO_TESTE ${ARQUIVOS_DE_TESTE})
    get_filename_component(NOME_TESTE ${ARQUIVO_TESTE} NAME_WE)
    string(REPLACE "teste" "teste-" ALVO_TESTE ${NOME_TESTE})
    add_executable(${ALVO_TESTE} ${ARQUIVO_TESTE} ${ARQUIVO_RECURSOS})
    target_link_libraries(${ALVO_TESTE} PRIVATE analisador-nucleo Qt6::Test)
    add_test(NAME ${ALVO_TESTE} COMMAND ${ALVO_TESTE})
endforeach()

# -----------------------------------------------------------------------------
# Copia os CSVs de demonstracao para junto do executavel, para a apresentacao
# funcionar sem depender do diretorio de onde o programa foi iniciado.
# -----------------------------------------------------------------------------
file(GLOB ARQUIVOS_CSV "${CMAKE_CURRENT_SOURCE_DIR}/dados/*.csv")
if(ARQUIVOS_CSV)
    add_custom_command(TARGET analisador-b3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:analisador-b3>/dados"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARQUIVOS_CSV} "$<TARGET_FILE_DIR:analisador-b3>/dados"
        COMMENT "Copiando CSVs de demonstracao para o diretorio do executavel")
endif()

message(STATUS "AnalisadorB3 ${PROJECT_VERSION} - Qt ${Qt6_VERSION} - build ${CMAKE_BUILD_TYPE}")
