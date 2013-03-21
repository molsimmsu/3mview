#include "transferfunctioneditor.h"

TransferFunctionEditor::TransferFunctionEditor()
  : func1_("func1", "Transfer function 1")
  , func2_("func2", "Transfer function 2")
  , func3_("func3", "Transfer function 3")
  , interactionCoarseness_("interactionCoarseness", "Interaction Coarseness", 1, 1, 10)
{
    addProperty(interactionCoarseness_);
	addProperty(func1_);
    addProperty(func2_);
    addProperty(func3_);
}

