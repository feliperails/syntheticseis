#include <gtest/gtest.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <domain/src/RickerWaveletCalculator.h>
#include <storage/src/reader/GrdSurfaceReader.h>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "DomainTestValues.h"

TEST(DomainTest, RickerWavelet)
{
    using namespace syntheticSeismic::domain;

    auto waveletCompare = DomainTestValues::rickerWavelet();

    RickerWaveletCalculator rickerCalculator;
    rickerCalculator.setStep(waveletCompare.getStep());
    rickerCalculator.setFrequency(waveletCompare.getFrequency());
    auto wavelet = rickerCalculator.extract();

    EXPECT_EQ(wavelet->getTraces().size(), waveletCompare.getTraces().size());

    if (wavelet->getTraces().size() == waveletCompare.getTraces().size())
    {
        double epsilon = std::pow(10, -14);
        for (size_t i = 0; i < wavelet->getTraces().size(); ++i)
        {
            EXPECT_LT(std::abs(wavelet->getTraces()[i] - waveletCompare.getTraces()[i]), epsilon);
        }
    }
}