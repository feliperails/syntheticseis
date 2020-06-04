#pragma once

#include <QString>
#include <H5Cpp.h>
#include <QString>
#include <QVariant>
#include <exception>
#include <memory>
#include <map>
#include "domain/src/RegularGrid.h"

namespace syntheticSeismic {
namespace storage {

template<class RasterType>
class RegularGridHdf5Writer
{
public:
    /// Construtor
    RegularGridHdf5Writer(QString pathFile, QString datasetName)
    {
        setPathFile(pathFile);
        setDatasetName(datasetName);
    }

    void setDatasetName(const QString &datasetName)
    {
        m_datasetName = datasetName;
    }

    QString getDatasetName() const
    {
        return m_datasetName;
    }

    QString getPathFile() const
    {
        return m_pathFile;
    }

    void setPathFile(QString pathFile){
        m_pathFile = pathFile;
    }

    void write(
            domain::RegularGrid<RasterType> &regularGrid,
            unsigned short version = 0,
            const std::map<QString, QVariant> &extraAttributes = std::map<QString, QVariant>()
        ) const
    {
        const H5std_string fileName(getPathFile().toStdString());
        H5::H5File file(fileName, H5F_ACC_TRUNC);

        // Salva o nome do dataset
        const H5std_string datasetName(getDatasetName().toStdString());
        // Dataset dimensions
        hsize_t dims[3];
        dims[0] = regularGrid.getNumberOfCellsInX();
        dims[1] = regularGrid.getNumberOfCellsInY();
        dims[2] = regularGrid.getNumberOfCellsInZ();
        H5::DataSpace dataspace(3, dims);
        // Create the dataset.

        H5::DataSet dataset = file.createDataSet(
                    datasetName,
                    std::is_floating_point<RasterType>::value ? H5::PredType::NATIVE_FLOAT : H5::PredType::NATIVE_INT,
                    dataspace
                );

        std::map<QString, QVariant> attributes;

        attributes["numberOfCellsInX"] = static_cast<int>(regularGrid.getNumberOfCellsInX());
        attributes["numberOfCellsInY"] = static_cast<int>(regularGrid.getNumberOfCellsInY());
        attributes["numberOfCellsInZ"] = static_cast<int>(regularGrid.getNumberOfCellsInZ());
        attributes["cellSizeInX"] = static_cast<double>(regularGrid.getCellSizeInX());
        attributes["cellSizeInY"] = static_cast<double>(regularGrid.getCellSizeInY());
        attributes["cellSizeInZ"] = static_cast<double>(regularGrid.getCellSizeInZ());
        attributes["noDataValue"] = static_cast<int>(regularGrid.getNoDataValue());

        auto &rectanglePoints = regularGrid.getRectanglePoints();
        for (size_t i = 0; i < rectanglePoints.size(); ++i)
        {
            QString nameX = "rectanglePoint_";
            nameX += QString::number(i);
            nameX += "_x";
            QString nameY = "rectanglePoint_";
            nameY += QString::number(i);
            nameY += "_y";

            attributes[nameX] = static_cast<double>(rectanglePoints[i].x);
            attributes[nameY] = static_cast<double>(rectanglePoints[i].y);
        }

        attributes["version"] = version;

        for (auto pair : extraAttributes)
        {
            attributes[pair.first] = pair.second;
        }

        hsize_t dimsAttributes[1] = { 1 };
        // Create the data space for the attribute.
        H5::DataSpace attrDataspace = H5::DataSpace(1, dimsAttributes);
        for (auto &pair : attributes)
        {
            const H5std_string attr(pair.first.toStdString());
            switch (static_cast<QMetaType::Type>(pair.second.type()))
            {
                case QMetaType::Type::Int:
                {
                    const int attrData[1] = { pair.second.toInt() };
                    H5::Attribute attribute = dataset.createAttribute(attr, H5::PredType::NATIVE_INT, attrDataspace);
                    attribute.write(H5::PredType::NATIVE_INT, attrData);
                    break;
                }
                case QMetaType::Type::Double:
                {
                    const double attrData[1] = { pair.second.toDouble() };
                    H5::Attribute attribute = dataset.createAttribute(attr, H5::PredType::NATIVE_DOUBLE, attrDataspace);
                    attribute.write(H5::PredType::NATIVE_DOUBLE, attrData);
                    break;
                }
                default:
                    QString text = "unexpected attribute ";
                    text += pair.second.typeName();
                    throw std::exception(text.toLocal8Bit().constData());
            }
        }

        // Data initialization.
        const auto &regularGridData = regularGrid.getData();
        auto data = new RasterType[regularGrid.getNumberOfCellsInX() * regularGrid.getNumberOfCellsInY() * regularGrid.getNumberOfCellsInZ()];
        size_t i = 0;
        for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
        {
            for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
            {
                for (size_t z = 0; z < regularGrid.getNumberOfCellsInZ(); ++z)
                {
                    data[i] = regularGridData[x][y][z];
                    ++i;
                }
            }
        }

        // Write the data to the dataset using default memory space, file
        // space, and transfer properties.
        dataset.write(data, std::is_floating_point<RasterType>::value ? H5::PredType::NATIVE_DOUBLE : H5::PredType::NATIVE_INT);
        dataset.close();

        delete[] data;

        file.close();
    }
private:
    QString m_pathFile;
    QString m_datasetName;
};

}
}
