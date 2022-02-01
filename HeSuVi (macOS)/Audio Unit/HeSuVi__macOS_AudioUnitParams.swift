//
//  HeSuVi__macOS_AudioUnitParams.swift
//  HeSuVi (macOS)
//
//  Created by Christopher Snowhill on 31 Jan 2022.
//

import Foundation

class HeSuVi__macOS_AudioUnitParams {

    let parameterTree: AUParameterTree
    // TODO: figure out how to deal with all of the errors that appear when I try to do this the right way.
//    let percentFormatter: NumberFormatter
//    let millisecondFormatter: MeasurementFormatter

    init(kernelAdapter: HeSuVi__macOS_DSPKernelAdapter) {
//        self.percentFormatter = NumberFormatter()
//        self.percentFormatter.numberStyle = .percent
//        self.percentFormatter.multiplier = 100
//        self.millisecondFormatter = MeasurementFormatter()

        // Create the audio unit's tree of parameters
        parameterTree = AUParameterTree.init()

        // Closure returning string representation of requested parameter value.
        parameterTree.implementorStringFromValueCallback = { param, value in
            return "?"
        }
    }

    func setParameterValues(speechConfidenceThresholdPct: AUValue, gateAttackDelay: AUValue) {
    }
}
