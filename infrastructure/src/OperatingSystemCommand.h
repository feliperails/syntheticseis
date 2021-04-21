#pragma once

#include <QString>
#include <QObject>
#include <QDir>
#include <vector>
#include "OperatingSystemReturnCommand.h"

namespace syntheticSeismic {
namespace infrastructure {
/** @brief Classe de Comandos do Sistema Operacional
 * Esta classe tem como objetivo fazer a execução de comandos via console com o sistema operacional
 */
class OperatingSystemCommand : QObject
{
public:
    /// Função para atualizar o número de perfis transitórios
    /// @param pathLibHome Caminho fisico do prgrama a ser executado
    /// @param programName Nome do Programa
    /// @param args Lista de argumentos do programa
    /// @return Retorna objeto com status e output da execução do progarama
    OperatingSystemReturnCommand runCommand(const QString program, const QStringList args);

    ///Destrutor
    virtual  ~OperatingSystemCommand() = default;
};

} // namespace infrastructure
} // namespace continental
