/*
This file is part of the project Sabrina
Copyright (C) 2020  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "envvars.h"

#include <cstdlib>


bool envVarIsSet( QString const & key ) {
	char * val = getenv( key.toStdString().c_str() );
	return val == nullptr;
}

QString getEnvVar( QString const & key ) {
	char * val = getenv( key.toStdString().c_str() );
	return (val == nullptr) ? QString("") : QString(val);
}
