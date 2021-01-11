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
class RegularGridHdf5Storage
{
public:
    /// Construtor
    RegularGridHdf5Storage(QString pathFile, QString datasetName)
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
        attributes["cellSizeInX"] = regularGrid.getCellSizeInX();
        attributes["cellSizeInY"] = regularGrid.getCellSizeInY();
        attributes["cellSizeInZ"] = regularGrid.getCellSizeInZ();
        attributes["noDataValue"] = static_cast<int>(regularGrid.getNoDataValue());
        // BEGIN rectanglePoints
        auto &rectanglePoints = regularGrid.getRectanglePoints();
        attributes["geometryInlineEndCrosslineEndX"] = rectanglePoints[0].x;
        attributes["geometryInlineEndCrosslineEndY"] = rectanglePoints[0].y;
        attributes["geometryInlineEndX"] = rectanglePoints[1].x;
        attributes["geometryInlineEndY"] = rectanglePoints[1].y;
        attributes["geometryOriginX"] = rectanglePoints[2].x;
        attributes["geometryOriginY"] = rectanglePoints[2].y;
        attributes["geometryOriginZ"] = regularGrid.getZTop();
        attributes["geometryCrosslineEndX"] = rectanglePoints[3].x;
        attributes["geometryCrosslineEndY"] = rectanglePoints[3].y;
        // END rectanglePoints
        attributes["zBottom"] = regularGrid.getZBottom();
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

    std::shared_ptr<domain::RegularGrid<RasterType>> read()
    {
        H5::H5File hdf5File(getPathFile().toLocal8Bit().constData(), H5F_ACC_RDWR);
        H5::DataSet dataset = hdf5File.openDataSet(getDatasetName().toStdString());

        auto attributes = readAttributes(dataset);

        auto numberOfCellsInX = static_cast<size_t>(attributes["numberOfCellsInX"].toInt());
        auto numberOfCellsInY = static_cast<size_t>(attributes["numberOfCellsInY"].toInt());
        auto numberOfCellsInZ = static_cast<size_t>(attributes["numberOfCellsInZ"].toInt());
        auto cellSizeInX = attributes["cellSizeInX"].toDouble();
        auto cellSizeInY = attributes["cellSizeInY"].toDouble();
        auto cellSizeInZ = attributes["cellSizeInZ"].toDouble();
        // BEGIN rectanglePoints
        std::array<geometry::Point2D, 4> rectanglePoints;
        rectanglePoints[0].x = attributes["geometryInlineEndCrosslineEndX"].toDouble();
        rectanglePoints[0].y = attributes["geometryInlineEndCrosslineEndY"].toDouble();
        rectanglePoints[1].x = attributes["geometryInlineEndX"].toDouble();
        rectanglePoints[1].y = attributes["geometryInlineEndY"].toDouble();
        rectanglePoints[2].x = attributes["geometryOriginX"].toDouble();
        rectanglePoints[2].y = attributes["geometryOriginY"].toDouble();
        auto zTop = attributes["geometryOriginZ"].toDouble();
        rectanglePoints[3].x = attributes["geometryCrosslineEndX"].toDouble();
        rectanglePoints[3].y = attributes["geometryCrosslineEndY"].toDouble();
        // END rectanglePoints
        auto zBottom = attributes["zBottom"].toDouble();
        auto noDataValue = static_cast<int>(attributes["noDataValue"].toInt());

        auto regularGrid = std::make_shared<domain::RegularGrid<RasterType>>(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                cellSizeInX, cellSizeInY, cellSizeInZ,
                rectanglePoints,
                zBottom,
                zTop,
                noDataValue,
                noDataValue
            );
        auto &regularGridData = regularGrid->getData();

        auto totalCells = numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ;
        auto data = new RasterType[totalCells];

        dataset.read(data, std::is_floating_point<RasterType>::value ? H5::PredType::NATIVE_DOUBLE : H5::PredType::NATIVE_INT);
        size_t i = 0;
        for (size_t x = 0; x < numberOfCellsInX; ++x)
        {
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                for (size_t z = 0; z < numberOfCellsInZ; ++z)
                {
                    regularGridData[x][y][z] = regularGridData[x][y][z];
                    ++i;
                }
            }
        }

        delete[] data;

        hdf5File.close();

        return regularGrid;
    }
private:
    QString m_pathFile;
    QString m_datasetName;

    std::map<QString, QVariant> readAttributes(H5::DataSet &dataset)
    {
        std::map<QString, QVariant> result;

        auto limit = static_cast<size_t>(dataset.getNumAttrs());
        for(size_t i = 0; i < limit; ++i)
        {
            H5::Attribute attribute = dataset.openAttribute(static_cast<const unsigned int>(i));
            switch (attribute.getDataType().getClass())
            {
                case H5T_INTEGER:
                {
                    int attrData[1];
                    attribute.read(H5::PredType::NATIVE_INT, attrData);
                    result[QString::fromStdString(attribute.getName())] = QVariant(static_cast<int>(attrData[0]));
                    break;
                }
                case H5T_FLOAT:
                {
                    double attrData[1];
                    attribute.read(H5::PredType::NATIVE_DOUBLE, attrData);
                    result[QString::fromStdString(attribute.getName())] = QVariant(static_cast<double>(attrData[0]));
                    break;
                }
                default:
                    throw std::exception("RegularGridHdf5Writer::readAttributes - Não foi possível identificar o tipo de atributo.");
            }
        }

        return result;
    }
};

}
}
