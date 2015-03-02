/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#if JUCE_INTEL
 #define JUCE_SNAP_TO_ZERO(n)    if (! (n < -1.0e-8 || n > 1.0e-8)) n = 0;
#else
 #define JUCE_SNAP_TO_ZERO(n)
#endif

//==============================================================================
IIRCoefficients::IIRCoefficients() noexcept
{
    zeromem (c, sizeof (c));
}

IIRCoefficients::~IIRCoefficients() noexcept {}

IIRCoefficients::IIRCoefficients (const IIRCoefficients& other) noexcept
{
    memcpy (c, other.c, sizeof (c));
}

IIRCoefficients& IIRCoefficients::operator= (const IIRCoefficients& other) noexcept
{
    memcpy (c, other.c, sizeof (c));
    return *this;
}

IIRCoefficients::IIRCoefficients (double c1, double c2, double c3,
                                  double c4, double c5, double c6) noexcept
{
    const double a = 1.0 / c4;

    c[0] = (float) (c1 * a);
    c[1] = (float) (c2 * a);
    c[2] = (float) (c3 * a);
    c[3] = (float) (c5 * a);
    c[4] = (float) (c6 * a);
}

IIRCoefficients IIRCoefficients::makeLowPass (const double sampleRate,
                                              const double frequency) noexcept
{
    jassert (sampleRate > 0);

    const double n = 1.0 / tan (double_Pi * frequency / sampleRate);
    const double nSquared = n * n;
    const double c1 = 1.0 / (1.0 + std::sqrt (2.0) * n + nSquared);

    return IIRCoefficients (c1,
                            c1 * 2.0f,
                            c1,
                            1.0,
                            c1 * 2.0 * (1.0 - nSquared),
                            c1 * (1.0 - std::sqrt (2.0) * n + nSquared));
}

IIRCoefficients IIRCoefficients::makeHighPass (const double sampleRate,
                                               const double frequency) noexcept
{
    const double n = tan (double_Pi * frequency / sampleRate);
    const double nSquared = n * n;
    const double c1 = 1.0 / (1.0 + std::sqrt (2.0) * n + nSquared);

    return IIRCoefficients (c1,
                            c1 * -2.0f,
                            c1,
                            1.0,
                            c1 * 2.0 * (nSquared - 1.0),
                            c1 * (1.0 - std::sqrt (2.0) * n + nSquared));
}

IIRCoefficients IIRCoefficients::makeLowShelf (const double sampleRate,
                                               const double cutOffFrequency,
                                               const double Q,
                                               const float gainFactor) noexcept
{
    jassert (sampleRate > 0);
    jassert (Q > 0);

    const double A = jmax (0.0f, std::sqrt (gainFactor));
    const double aminus1 = A - 1.0;
    const double aplus1 = A + 1.0;
    const double omega = (double_Pi * 2.0 * jmax (cutOffFrequency, 2.0)) / sampleRate;
    const double coso = std::cos (omega);
    const double beta = std::sin (omega) * std::sqrt (A) / Q;
    const double aminus1TimesCoso = aminus1 * coso;

    return IIRCoefficients (A * (aplus1 - aminus1TimesCoso + beta),
                            A * 2.0 * (aminus1 - aplus1 * coso),
                            A * (aplus1 - aminus1TimesCoso - beta),
                            aplus1 + aminus1TimesCoso + beta,
                            -2.0 * (aminus1 + aplus1 * coso),
                            aplus1 + aminus1TimesCoso - beta);
}

IIRCoefficients IIRCoefficients::makeHighShelf (const double sampleRate,
                                                const double cutOffFrequency,
                                                const double Q,
                                                const float gainFactor) noexcept
{
    jassert (sampleRate > 0);
    jassert (Q > 0);

    const double A = jmax (0.0f, std::sqrt (gainFactor));
    const double aminus1 = A - 1.0;
    const double aplus1 = A + 1.0;
    const double omega = (double_Pi * 2.0 * jmax (cutOffFrequency, 2.0)) / sampleRate;
    const double coso = std::cos (omega);
    const double beta = std::sin (omega) * std::sqrt (A) / Q;
    const double aminus1TimesCoso = aminus1 * coso;

    return IIRCoefficients (A * (aplus1 + aminus1TimesCoso + beta),
                            A * -2.0 * (aminus1 + aplus1 * coso),
                            A * (aplus1 + aminus1TimesCoso - beta),
                            aplus1 - aminus1TimesCoso + beta,
                            2.0 * (aminus1 - aplus1 * coso),
                            aplus1 - aminus1TimesCoso - beta);
}

IIRCoefficients IIRCoefficients::makePeakFilter (const double sampleRate,
                                                 const double centreFrequency,
                                                 const double Q,
                                                 const float gainFactor) noexcept
{
    jassert (sampleRate > 0);
    jassert (Q > 0);

    const double A = jmax (0.0f, std::sqrt (gainFactor));
    const double omega = (double_Pi * 2.0 * jmax (centreFrequency, 2.0)) / sampleRate;
    const double alpha = 0.5 * std::sin (omega) / Q;
    const double c2 = -2.0 * std::cos (omega);
    const double alphaTimesA = alpha * A;
    const double alphaOverA = alpha / A;

    return IIRCoefficients (1.0 + alphaTimesA,
                            c2,
                            1.0 - alphaTimesA,
                            1.0 + alphaOverA,
                            c2,
                            1.0 - alphaOverA);
}

//==============================================================================
IIRFilter::IIRFilter()
    : active (false), v1 (0), v2 (0)
{
}

IIRFilter::IIRFilter (const IIRFilter& other)
    : active (other.active), v1 (0), v2 (0)
{
    const SpinLock::ScopedLockType sl (other.processLock);
    coefficients = other.coefficients;
}

IIRFilter::~IIRFilter()
{
}

//==============================================================================
void IIRFilter::makeInactive() noexcept
{
    const SpinLock::ScopedLockType sl (processLock);
    active = false;
}

void IIRFilter::setCoefficients (const IIRCoefficients& newCoefficients) noexcept
{
    const SpinLock::ScopedLockType sl (processLock);

    coefficients = newCoefficients;
    active = true;
}

//==============================================================================
void IIRFilter::reset() noexcept
{
    const SpinLock::ScopedLockType sl (processLock);
    v1 = v2 = 0;
}

float IIRFilter::processSingleSampleRaw (const float in) noexcept
{
    float out = coefficients.c[0] * in + v1;

    JUCE_SNAP_TO_ZERO (out);

    v1 = coefficients.c[1] * in - coefficients.c[3] * out + v2;
    v2 = coefficients.c[2] * in - coefficients.c[4] * out;

    return out;
}

void IIRFilter::processSamples (float* const samples,
                                const int numSamples) noexcept
{
    const SpinLock::ScopedLockType sl (processLock);

    if (active)
    {
        const float c0 = coefficients.c[0];
        const float c1 = coefficients.c[1];
        const float c2 = coefficients.c[2];
        const float c3 = coefficients.c[3];
        const float c4 = coefficients.c[4];
        float lv1 = v1, lv2 = v2;

        for (int i = 0; i < numSamples; ++i)
        {
            const float in = samples[i];
            const float out = c0 * in + lv1;
            samples[i] = out;

            lv1 = c1 * in - c3 * out + lv2;
            lv2 = c2 * in - c4 * out;
        }

        JUCE_SNAP_TO_ZERO (lv1);  v1 = lv1;
        JUCE_SNAP_TO_ZERO (lv2);  v2 = lv2;
    }
}

#undef JUCE_SNAP_TO_ZERO