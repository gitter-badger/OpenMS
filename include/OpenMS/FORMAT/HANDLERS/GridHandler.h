// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// --------------------------------------------------------------------------
//                   OpenMS Mass Spectrometry Framework
// --------------------------------------------------------------------------
//  Copyright (C) 2003-2007 -- Oliver Kohlbacher, Knut Reinert
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------
// $Maintainer: Eva Lange $
// --------------------------------------------------------------------------

#ifndef OPENMS_FORMAT_HANDLERS_GRIDHANDLER_H
#define OPENMS_FORMAT_HANDLERS_GRIDHANDLER_H

#include <OpenMS/DATASTRUCTURES/DPosition.h>
#include <OpenMS/FORMAT/HANDLERS/XMLHandler.h>
#include <OpenMS/FORMAT/Param.h>
#include <OpenMS/DATASTRUCTURES/String.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/Grid.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/GridCell.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/BaseMapping.h>

// all implementations of class BaseMapping must be
// included here
#include <OpenMS/ANALYSIS/MAPMATCHING/LinearMapping.h>

// STL includes
#include <iostream>
#include <valarray>
#include <string>

#include <xercesc/sax2/Attributes.hpp>

namespace OpenMS
{
  namespace Internal
  {

    /** @brief XML Handler for a vector of grid cells including their transformations.
      
        This is a simplified version of class FeatureMapHandler. We explicitly allow
        several tagtypes even if just one type is used in this implementation (for
        details see class member further below). Therefore this class can be extended
        in the future in order to save meta information with the grid such as information
        about the experiment etc.
        
        @note A grid cell can have different transformations for each dimension.
        If you want this XML handler class to support other transformations than the
        linear one, you must register this class with the handler. For details, have a look
        at registerMappings_() .
     */
    class GridHandler
          : public XMLHandler
    {
    public:

      typedef GridCell::MappingVector MappingVector;

      /**@name Constructors and destructor */
      //@{
      ///
      GridHandler(Grid& grid, const String& filename);

      ///
      GridHandler(const Grid& grid, const String& filename);

      ///
      virtual ~GridHandler()
      {}
      //@}

      // Docu in base class
      virtual void startElement(const XMLCh* const /*uri*/, const XMLCh* const /*local_name*/, const XMLCh* const qname, const xercesc::Attributes& attributes);

      // Docu in base class
      virtual void characters(const XMLCh* const chars, unsigned int /*length*/);

      // Docu in base class
      virtual void endElement(const XMLCh* const /*uri*/, const XMLCh* const /*local_name*/, const XMLCh* const qname);

      /// Print the contents to a stream
      void writeTo(std::ostream& os);

    protected:

      std::vector<String> tagsVector_;

      /// Maps to assoziate Strings with enumeration values
      enum MapType {  TAGMAP };
      typedef std::map<std::string,int> Map;
      static const int MAP_NUM = 1;
      Map maps[MAP_NUM];

      /// Vector of grid cell to be read
      Grid* grid_;
      /// Vector of pairs to be written
      const Grid* cgrid_;

      /// The tags we expect to encounter
      enum Tags { CELLLIST, CELL, FIRSTPOSITION, SECONDPOSITION,
                  FPOSITION, SPOSITION, MAPPINGLIST, MAPPING, PARAM };

      static const int TAG_NUM = 9;

      /// Indicates which tag is currently parsed
      bool in_tag_[TAG_NUM];

      // temporary datastructures to hold parsed data
      GridCell* cell_;
      BaseMapping* mapping_;
      Param* param_;

      UnsignedInt current_fcoord_;
      UnsignedInt current_scoord_;

      std::map<String,BaseMapping* > mapping_instances;

      inline void fillMaps_()
      {
        fillMap_(maps[TAGMAP], tagsVector_);
      }

      /// mapping types must be registred with the handler class
      inline void registerMappings_()
      {
        // insert new mappings (transformations) here.
        mapping_instances["LinearMapping"] = new LinearMapping();
      }

      /// @brief Find value in the given map
      /// if not found: fatal error or warning message
      inline int useMap_(MapType type, String value, bool fatal=true, const char* message="")
      {
        Map::const_iterator it =  maps[type].find(value);
        if (it == maps[type].end())
        {
          if (fatal)
          {
            const xercesc::Locator* loc = 0;
            setDocumentLocator(loc);
            String message = String("Error in enumerated value \"") + value + "\"";
            error(xercesc::SAXParseException(xercesc::XMLString::transcode(message.c_str()), *loc ));
          }
          else if (message != "")
          {
            const xercesc::Locator* loc = 0;
            setDocumentLocator(loc);
            String message = String("Unhandled ") + message + "\"" + value + "\"";
            warning(xercesc::SAXParseException(xercesc::XMLString::transcode(message.c_str()), *loc ));
          }
        }
        else
        {
          return it->second;
        }
        return 0;
      }

      ///  @brief Create map from the given set of strings
      inline void fillMap_(Map& dict, std::vector<String> array)
      {
        for (UnsignedInt i=0; i<array.size(); i++)
        {
          dict[ std::string(array[i]) ] = i;
        }
      }

      /** @brief Set constants of XML handler */
      inline void setConstants_()
      {
        char* tags[]
        = {"celllist", "cell", "first", "second", "fposition",
           "sposition", "mappinglist", "mapping", "param"};
        fillVector_(tagsVector_,tags,TAG_NUM);
      }

      inline void fillVector_(std::vector<String>& vec, char* contents[], int nr)
      {
        for (int i=0; i<nr;i++) vec.push_back(contents[i]);
      }

    }
    ; // end of class GridHandler

  } // namespace Internal
} // namespace OpenMS

#endif
