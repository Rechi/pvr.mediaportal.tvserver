/*
 *      Copyright (C) 2005-2011 Team XBMC
 *      http://www.xbmc.org
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>
#include <vector>
#include <stdio.h>

using namespace std;

#include "epg.h"
#include "utils.h"
#include "client.h"
#include "DateTime.h"

using namespace ADDON;

cEpg::cEpg()
{
  m_genretable       = NULL;
  Reset();
}

cEpg::~cEpg()
{
}

void cEpg::Reset()
{
  m_genre.clear();
  m_title.clear();
  m_description.clear();
  m_episodePart.clear();
  m_episodeName.clear();

  m_uid             = 0;
  m_originalAirDate = 0;
  m_duration        = 0;
  m_genre_type      = 0;
  m_genre_subtype   = 0;
  m_seriesNumber    = 0;
  m_episodeNumber   = 0;
  m_starRating      = 0;
  m_parentalRating  = 0;
}

bool cEpg::ParseLine(string& data)
{
  try
  {
    vector<string> epgfields;

    Tokenize(data, epgfields, "|");

    if( epgfields.size() >= 5 )
    {
      //XBMC->Log(LOG_DEBUG, "%s: %s", epgfields[0].c_str(), epgfields[2].c_str());
      // field 0 = start date + time
      // field 1 = end   date + time
      // field 2 = title
      // field 3 = description
      // field 4 = genre string
      // field 5 = idProgram (int)
      // field 6 = idChannel (int)
      // field 7 = seriesNum (string)
      // field 8 = episodeNumber (string)
      // field 9 = episodeName (string)
      // field 10 = episodePart (string)
      // field 11 = originalAirDate (date + time)
      // field 12 = classification (string)
      // field 13 = starRating (int)
      // field 14 = parentalRating (int)

      if( m_startTime.SetFromDateTime(epgfields[0]) == false )
      {
        XBMC->Log(LOG_ERROR, "cEpg::ParseLine: Unable to convert start time '%s' into date+time", epgfields[0].c_str());
        return false;
      }

      if( m_endTime.SetFromDateTime(epgfields[1]) == false )
      {
        XBMC->Log(LOG_ERROR, "cEpg::ParseLine: Unable to convert end time '%s' into date+time", epgfields[1].c_str());
        return false;
      }

      m_duration  = m_endTime - m_startTime;

      m_title = epgfields[2];
      m_description = epgfields[3];
      m_genre = epgfields[4];
      if (m_genretable) m_genretable->GenreToTypes(m_genre, m_genre_type, m_genre_subtype);

      if( epgfields.size() >= 15 )
      {
        // Since TVServerXBMC v1.x.x.104
        m_uid = (unsigned int) atol(epgfields[5].c_str());
        m_seriesNumber = atoi(epgfields[7].c_str());
        m_episodeNumber = atoi(epgfields[8].c_str());
        m_episodeName = epgfields[9];
        m_episodePart = epgfields[10];
        m_starRating = atoi(epgfields[13].c_str());
        m_parentalRating = atoi(epgfields[14].c_str());

        //originalAirDate
        if( m_originalAirDate.SetFromDateTime(epgfields[11]) == false )
        {
          XBMC->Log(LOG_ERROR, "cEpg::ParseLine: Unable to convert original air date '%s' into date+time", epgfields[11].c_str());
          return false;
        }
      }

      return true;
    }
  }
  catch(std::exception &e)
  {
    XBMC->Log(LOG_ERROR, "Exception '%s' during parse EPG data string.", e.what());
  }

  return false;
}

void cEpg::SetGenreTable(CGenreTable* genretable)
{
  m_genretable = genretable;
}

time_t cEpg::StartTime(void) const
{
  time_t retval = m_startTime.GetAsTime();
  return retval;
}

time_t cEpg::EndTime(void) const
{
  time_t retval = m_endTime.GetAsTime();
  return retval;
}

time_t cEpg::OriginalAirDate(void) const
{
  time_t retval = m_endTime.GetAsTime();
  return retval;
}

const char *cEpg::PlotOutline(void) const
{
  if (m_episodeName.empty())
    return m_title.c_str();
  else
    return m_episodeName.c_str();
}
