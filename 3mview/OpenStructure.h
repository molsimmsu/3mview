#ifndef OPENSTRUCTURE_H
#define OPENSTRUCTURE_H

#include "ost/mol/entity_handle.hh"
#include "ost/mol/chain_handle.hh"
#include "ost/mol/residue_handle.hh"
#include "ost/mol/atom_handle.hh"
#include "ost/mol/editor_base.hh"
#include "ost/mol/xcs_editor.hh"
#include "ost/mol/chem_class.hh"

#include "ost/gui/gl_win.hh"
#include "ost/gui/gl_canvas.hh"

#include "ost/gfx/scene.hh"
#include "ost/gfx/map_iso.hh"
#include "ost/gfx/entity.hh"
#include "ost/gfx/primitives.hh"
#include "ost/gfx/impl/backbone_trace.hh"

#include "ost/io/mol/load_entity.hh"
#include "ost/io/img/load_map.hh"

#include "ost/geom/vec3.hh"

using namespace ost::gfx;
using namespace ost::mol;
using namespace ost::gui;
using namespace ost::img;
using namespace ost::io;
using namespace geom;

#endif // OPENSTRUCTURE_H
