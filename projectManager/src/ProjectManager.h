#ifndef INC_INVERTSEIS_PROJECTMANAGER_H
#define INC_INVERTSEIS_PROJECTMANAGER_H

#include <QObject>

namespace invertseis {
namespace projectManager {

class ProjectManager : public QObject
{
    Q_OBJECT
public:
    ProjectManager();

    static ProjectManager& instance();
};

} // namespace projectManager
} // namespace invertseis
#endif
