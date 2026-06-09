AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 28
   name "Tinnitus"
   tl 0 768
   children {
    7
   }
  }
  IOPInputVariableClass {
   id 29
   name "GAboveFreshWater"
   tl 0 256
   children {
    9
   }
   varName "GAboveFreshWater"
   varResource "{A60F08955792B575}Sounds/_SharedData/Variables/GlobalVariables.conf"
  }
  IOPInputVariableClass {
   id 30
   name "GAboveSea"
   children {
    3
   }
   varName "GAboveSea"
   varResource "{A60F08955792B575}Sounds/_SharedData/Variables/GlobalVariables.conf"
  }
  IOPInputVariableClass {
   id 31
   name "CharacterLifeState"
   tl 0 512
   children {
    12
   }
   varName "CharacterLifeState"
   varResource "{A60F08955792B575}Sounds/_SharedData/Variables/GlobalVariables.conf"
  }
 }
 Ops {
  IOPItemOpInterpolateClass {
   id 3
   name "Interpolate GAboveSea"
   tl 255.13 0
   children {
    7 10
   }
   inputs {
    ConnectionClass "30:0" {
     id 30
     port 0
    }
   }
   "X max" -0.3
  }
  IOPItemOpMaxClass {
   id 7
   name "Max LPF"
   tl 705 264.87
   children {
    32
   }
   inputs {
    ConnectionClass "13:0" {
     id 13
     port 0
    }
    ConnectionClass "3:0" {
     id 3
     port 0
    }
    ConnectionClass "28:0" {
     id 28
     port 0
    }
    ConnectionClass "9:0" {
     id 9
     port 0
    }
   }
  }
  IOPItemOpInterpolateClass {
   id 9
   name "Interpolate GAboveFreshWater"
   tl 255.13 256
   children {
    7 10
   }
   inputs {
    ConnectionClass "29:0" {
     id 29
     port 0
    }
   }
   "X max" -0.1
  }
  IOPItemOpMaxClass {
   id 10
   name "Max Under Water"
   tl 768 0
   children {
    33
   }
   inputs {
    ConnectionClass "9:0" {
     id 9
     port 0
    }
    ConnectionClass "3:0" {
     id 3
     port 0
    }
   }
  }
  IOPItemOpConvertorClass {
   id 12
   name "Converter 12"
   tl 256 512
   children {
    13
   }
   inputs {
    ConnectionClass "31:0" {
     id 31
     port 0
    }
   }
   Intervals {
    IOPItemOpConvertorRange "0" {
     min 1
     max 10
     "out" 0.8
    }
    IOPItemOpConvertorRange "1" {
     min 10
     max 11
     "out" 1
    }
   }
  }
  IOPItemOpSmootherClass {
   id 13
   name "Smoother"
   tl 512 512
   children {
    7
   }
   inputs {
    ConnectionClass "12:0" {
     id 12
     port 0
    }
   }
  }
  IOPItemOpInterpolateClass {
   id 32
   name "Interpolate 32"
   tl 870.039 351.1
   children {
    4
   }
   inputs {
    ConnectionClass "7:0" {
     id 7
     port 0
    }
   }
   EnableCustomCurve 1
   CustomCurve {
    CurvePoint "1" {
    }
    CurvePoint "2" {
     X 0.39939
     Y 1
    }
    CurvePoint "3" {
     X 1
     Y 1
    }
   }
  }
  IOPItemOpInterpolateClass {
   id 33
   name "Interpolate 32"
   tl 888.039 90.1
   children {
    2
   }
   inputs {
    ConnectionClass "10:0" {
     id 10
     port 0
    }
   }
   EnableCustomCurve 1
   CustomCurve {
    CurvePoint "1" {
    }
    CurvePoint "2" {
     X 0.40122
     Y 1
    }
    CurvePoint "3" {
     X 1
     Y 1
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 2
   name "UnderWater_T"
   tl 1024 0
   input 33
  }
  IOPItemOutputClass {
   id 4
   name "LPF_W"
   tl 1042 259
   input 32
  }
 }
 compiled IOPCompiledClass "{692E887229501515}" {
  visited {
   389 519 647 261 7 133 263 395 6 5 147 134
  }
  ins {
   IOPCompiledIn "{692E8872295014F8}" {
    data {
     1 65539
    }
   }
   IOPCompiledIn "{692E8872295014CF}" {
    data {
     1 131075
    }
   }
   IOPCompiledIn "{692E8872295014D6}" {
    data {
     1 3
    }
   }
   IOPCompiledIn "{692E8872295014D8}" {
    data {
     1 262147
    }
   }
  }
  ops {
   IOPCompiledOp "{692E8872295014BF}" {
    data {
     2 65539 196611 2 131072 0
    }
   }
   IOPCompiledOp "{692E887229501488}" {
    data {
     1 65538 8 327681 0 1 0 0 0 131073 0
    }
   }
   IOPCompiledOp "{692E88722950149A}" {
    data {
     2 65539 196611 2 65536 0
    }
   }
   IOPCompiledOp "{692E88722950149D}" {
    data {
     1 2 4 131073 0 1 0
    }
   }
   IOPCompiledOp "{692E887229501465}" {
    data {
     1 327683 2 196608 0
    }
   }
   IOPCompiledOp "{692E887229501470}" {
    data {
     1 65539 2 262145 0
    }
   }
  }
  outs {
   IOPCompiledOut "{692E887229501440}" {
    data {
     0
    }
   }
   IOPCompiledOut "{692E887229501449}" {
    data {
     0
    }
   }
  }
  processed 12
  version 2
  ins_reeval_list {
   1 2 3
  }
  ops_reeval_list {
   5
  }
 }
 Input_Order {
  ItemDetailListItemClass Tinnitus {
   Name "Tinnitus"
   Id 28
  }
 }
 Output_Order {
  ItemDetailListItemClass UnderWater_T {
   Name "UnderWater_T"
   Id 2
  }
  ItemDetailListItemClass LPF_W {
   Name "LPF_W"
   Id 4
  }
 }
}