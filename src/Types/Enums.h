#ifndef ENUMS_H_
#define ENUMS_H_

#include <stdint.h>

/*!\brief This namespace contains all enumerated types within Mogu. For
 * the ease of maintainability, they are split into different files. This does
 * cause some problems with Eclipse if editing the enums or their parsers, so
 * if Eclipse starts going insane, just rebuild the index.
 */
namespace Enums{

#include <Types/E_Signals.h>
#include <Types/E_Submissions.h>
#include <Types/E_NodeValues.h>
#include <Types/E_Widgets.h>
#include <Types/E_WidgetStyling.h>
#include <Types/E_Validators.h>


} // <-- namespace Enums

#endif // ENUMS_H_
