//
//  AudioUnitViewController.swift
//  HeSuVi (macOS)
//
//  Created by Christopher Snowhill on 31 Jan 2022.
//

import CoreAudioKit

public class AudioUnitViewController: AUViewController, AUAudioUnitFactory {
    var audioUnit: AUAudioUnit?
    
    public override func viewDidLoad() {
        super.viewDidLoad()
        
        if audioUnit == nil {
            return
        }
        
        // Get the parameter tree and add observers for any parameters that the UI needs to keep in sync with the AudioUnit
    }
    
    public func createAudioUnit(with componentDescription: AudioComponentDescription) throws -> AUAudioUnit {
        audioUnit = try HeSuVi__macOS_AudioUnit(componentDescription: componentDescription, options: [])
        
        return audioUnit!
    }
    
}
