#include "ProjectManager.h"

#include <QPointer>

namespace invertseis {
namespace projectManager {

namespace {
static QPointer<ProjectManager> s_instance = nullptr;
}

ProjectManager::ProjectManager()
{
    Q_ASSERT_X(s_instance.isNull(), "ProjectManager::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    s_instance = QPointer<ProjectManager>(this);
}

void ProjectManager::init()
{
}

ProjectManager& ProjectManager::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "ProjectManager::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    return *s_instance;
}

}
}
