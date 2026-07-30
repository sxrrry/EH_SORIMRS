AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 1
   name "TransmissionQuality"
   tl -351.778 176.889
   children {
    6 7 14
   }
   value 1
  }
  IOPInputValueClass {
   id 9
   name "Vol Min [dB]"
   tl -346.364 378.182
   children {
    10
   }
   value -60
  }
  IOPInputValueClass {
   id 16
   name "Vol Min [dB]"
   tl -345.795 550
   children {
    17
   }
   value -12
  }
 }
 Ops {
  IOPItemOpInterpolateClass {
   id 6
   name "Interpolate 1"
   tl 34.85 99.6
   children {
    5
   }
   inputs {
    ConnectionClass "1:0" {
     id 1
     port 0
    }
   }
   EnableCustomCurve 1
   CustomCurve {
    CurvePoint "1" {
     X 0
     Y 1
     Shape Linear
     Modifier 1
    }
    CurvePoint "2" {
     X 0.22165
     Y 0.63652
     Shape Linear
     Modifier 1
    }
    CurvePoint "3" {
     X 0.49944
     Y 0.5249
     Shape Linear
     Modifier 1
    }
    CurvePoint "4" {
     X 1
     Y 0.5
     Shape Linear
     Modifier 1
    }
   }
  }
  IOPItemOpInterpolateClass {
   id 7
   name "Interpolate 1"
   tl 39.867 285.781
   children {
    8
   }
   inputs {
    ConnectionClass "10:4" {
     id 10
     port 4
    }
    ConnectionClass "1:0" {
     id 1
     port 0
    }
   }
   EnableCustomCurve 1
   CustomCurve {
    CurvePoint "1" {
     X 0
     Y 1
     Shape Linear
     Modifier 1
    }
    CurvePoint "2" {
     X 0.23755
     Y 0.30115
     Shape Linear
     Modifier 1
    }
    CurvePoint "3" {
     X 0.53197
     Y 0.03
     Shape Linear
     Modifier 1
    }
    CurvePoint "4" {
     X 1
     Y 0.01
     Shape Linear
     Modifier 1
    }
   }
  }
  SignalOpDb2GainClass {
   id 10
   name "Db2Gain 10"
   tl -157.273 378.182
   children {
    7
   }
   inputs {
    ConnectionClass "9:0" {
     id 9
     port 0
    }
   }
  }
  IOPItemOpInterpolateClass {
   id 14
   name "Interpolate 14"
   tl 41.528 477.639
   children {
    12
   }
   inputs {
    ConnectionClass "17:3" {
     id 17
     port 3
    }
    ConnectionClass "1:0" {
     id 1
     port 0
    }
   }
   EnableCustomCurve 1
   CustomCurve {
    CurvePoint "1" {
     X 0
     Y 0.25
     Shape Linear
     Modifier 1
    }
    CurvePoint "2" {
     X 0.25
     Y 0.6
     Shape Linear
     Modifier 1
    }
    CurvePoint "3" {
     X 0.6
     Y 1
     Shape Linear
     Modifier 1
    }
    CurvePoint "4" {
     X 1
     Y 1
     Shape Linear
     Modifier 1
    }
   }
  }
  SignalOpDb2GainClass {
   id 17
   name "Db2Gain 10"
   tl -155.593 548.75
   children {
    14
   }
   inputs {
    ConnectionClass "16:0" {
     id 16
     port 0
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 5
   name "Quality_W"
   tl 227 95
   input 6
  }
  IOPItemOutputClass {
   id 8
   name "Noise_V"
   tl 227 281
   input 7
  }
  IOPItemOutputClass {
   id 12
   name "Radio_V"
   tl 227 477.287
   input 14
  }
 }
 Input_Order {
  ItemDetailListItemClass TransmissionQuality {
   Name "TransmissionQuality"
   Id 1
  }
 }
 Output_Order {
  ItemDetailListItemClass Quality_W {
   Name "Quality_W"
   Id 5
  }
  ItemDetailListItemClass Noise_V {
   Name "Noise_V"
   Id 8
  }
  ItemDetailListItemClass Radio_V {
   Name "Radio_V"
   Id 12
  }
 }
}