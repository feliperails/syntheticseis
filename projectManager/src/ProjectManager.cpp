#include "ProjectManager.h"

#include <QPointer>

namespace invertseis {
namespace projectManager {

namespace {
static QPointer<ProjectManager> s_instance = nullptr;
}

ProjectManager::ProjectManager()
{
    Q_ASSERT_X(s_instance.isNull(), "ProjectManager::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<ProjectManager>(this);
}

ProjectManager& ProjectManager::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "ProjectManager::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

}
}
