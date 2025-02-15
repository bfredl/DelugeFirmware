/*
 * Copyright © 2019-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
*/

#include "model/consequence/consequence_output_existence.h"
#include "model/song/song.h"
#include "hid/display/numeric_driver.h"
#include "model/output.h"
#include "model/model_stack.h"

ConsequenceOutputExistence::ConsequenceOutputExistence(Output* newOutput, int newType) {
	output = newOutput;
	type = newType;
}

// TODO: wait a minute, do we have a memory leak here? Never deletes the Output?

int ConsequenceOutputExistence::revert(int time, ModelStack* modelStack) {
	if (time != type) { // Re-create

		modelStack->song->addOutput(output, true);
	}

	else { // Re-delete

		outputIndex = modelStack->song->removeOutputFromMainList(output);
		if (ALPHA_OR_BETA_VERSION && outputIndex == -1) numericDriver.freezeWithError("E263");

		output->prepareForHibernationOrDeletion();
	}

	return NO_ERROR;
}
