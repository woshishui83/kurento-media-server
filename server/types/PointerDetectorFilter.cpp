/*
 * PointerDetectorFilter.cpp - Kurento Media Server
 *
 * Copyright (C) 2013 Kurento
 * Contact: Miguel París Díaz <mparisdiaz@gmail.com>
 * Contact: José Antonio Santos Cadenas <santoscadenas@kurento.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PointerDetectorFilter.hpp"

#include "mediaEvents_types.h"

#include "protocol/TBinaryProtocol.h"
#include "transport/TBufferTransports.h"

#define GST_CAT_DEFAULT kurento_pointer_detector_filter
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);
#define GST_DEFAULT_NAME "KurentoPointerDetectorFilter"

using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::protocol::TBinaryProtocol;

namespace kurento
{

void
pointerDetector_receive_message (GstBus *bus, GstMessage *message, gpointer pointerDetector)
{
  //TODO: Implement
}

PointerDetectorFilter::PointerDetectorFilter (std::shared_ptr<MediaPipeline> parent) : Filter (parent, FilterType::type::POINTER_DETECTOR_FILTER)
{
  element = gst_element_factory_make ("filterelement", NULL);

  g_object_set (element, "filter-factory", "pointerdetector", NULL);
  g_object_ref (element);
  gst_bin_add (GST_BIN (parent->pipeline), element);
  gst_element_sync_state_with_parent (element);

  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (parent->pipeline) );
  GstElement *pointerDetector;

  g_object_get (G_OBJECT (element), "filter", &pointerDetector, NULL);

  this->pointerDetector = pointerDetector;

  bus_handler_id = g_signal_connect (bus, "message", G_CALLBACK (pointerDetector_receive_message), this);
  g_object_unref (bus);
  // There is no need to reference pointerdetector because its life cycle is the same as the filter life cycle
  g_object_unref (pointerDetector);
}

PointerDetectorFilter::~PointerDetectorFilter() throw ()
{
  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE ( ( (std::shared_ptr<MediaPipeline> &) parent)->pipeline) );
  g_signal_handler_disconnect (bus, bus_handler_id);
  g_object_unref (bus);

  gst_bin_remove (GST_BIN ( ( (std::shared_ptr<MediaPipeline> &) parent)->pipeline), element);
  gst_element_set_state (element, GST_STATE_NULL);
  g_object_unref (element);
}

PointerDetectorFilter::StaticConstructor PointerDetectorFilter::staticConstructor;

PointerDetectorFilter::StaticConstructor::StaticConstructor()
{
  GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0,
      GST_DEFAULT_NAME);
}

} // kurento