//
//  MappingUnprivate.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 18/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#define protected public
#define private public
#include UNPRIVATE_HEADER
#undef private
#undef protected

#undef UNPRIVATE_HEADER
