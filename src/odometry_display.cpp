
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>

#include <tf/transform_listener.h>

#include <rviz/visualization_manager.h>
#include <rviz/properties/color_property.h>
#include <rviz/properties/float_property.h>
#include <rviz/properties/ros_topic_property.h>
#include <rviz/frame_manager.h>

#include "covariance_visual.h"

#include "odometry_display.h"

namespace rviz_plugin_covariance
{

OdometryDisplay::OdometryDisplay()
{
  color_property_ = new rviz::ColorProperty( "Color", QColor( 204, 51, 204),
                                             "Color to draw the ellipsoid.",
                                             this, SLOT( updateColorAndAlpha() ));

  alpha_property_ = new rviz::FloatProperty( "Alpha", 1.0,
                                             "0 is fully transparent, 1.0 is fully opaque.",
                                             this, SLOT( updateColorAndAlpha() ));

  scale_property_ = new rviz::FloatProperty( "Scale", 1.0,
                                             "Ellipsoid scale factor.",
                                             this, SLOT( updateScale() ));

  show_axis_property_ = new rviz::BoolProperty( "Axis", true,
                                                "Show odometry axis.",
                                                this, SLOT( updateShowAxis() ));

  use_6dof_property_ = new rviz::BoolProperty( "6DOF", false,
                                               "Use 6DOF (x, y, z, roll, pitch, yaw) or 3DOF (x, y, yaw).",
                                               this, SLOT( updateUse6DOF() ));

  updateColorAndAlpha();
  updateScale();
  updateShowAxis();
  updateUse6DOF();
}

void OdometryDisplay::onInitialize()
{
  MFDClass::onInitialize();
}

OdometryDisplay::~OdometryDisplay()
{
}

void OdometryDisplay::reset()
{
  MFDClass::reset();
}

void OdometryDisplay::updateColorAndAlpha()
{
  const float alpha = alpha_property_->getFloat();
  const Ogre::ColourValue color = color_property_->getOgreColor();

  if (visual_)
  {
    visual_->setColor(color.r, color.g, color.b, alpha);
  }
}

void OdometryDisplay::updateScale()
{
  const float scale = scale_property_->getFloat();

  if (visual_)
  {
    visual_->setScale(scale);
  }
}

void OdometryDisplay::updateShowAxis()
{
  const bool show_axis = show_axis_property_->getBool();

  if (visual_)
  {
    visual_->setShowAxis(show_axis);
  }
}

void OdometryDisplay::updateUse6DOF()
{
  const bool use_6dof = use_6dof_property_->getBool();

  if (visual_)
  {
    visual_->setUse6DOF(use_6dof);
  }
}

void OdometryDisplay::processMessage(const nav_msgs::OdometryConstPtr& msg)
{
  Ogre::Quaternion orientation;
  Ogre::Vector3 position;
  if (!context_->getFrameManager()->getTransform( msg->header.frame_id,
                                                  msg->header.stamp,
                                                  position, orientation ))
  {
    ROS_DEBUG( "Error transforming from frame '%s' to frame '%s'",
               msg->header.frame_id.c_str(), qPrintable( fixed_frame_ ) );
    return;
  }

  if (!visual_)
  {
    visual_.reset(new CovarianceVisual(context_->getSceneManager(), scene_node_));
  }

  visual_->setMessage(msg);
  visual_->setFramePosition(position);
  visual_->setFrameOrientation(orientation);

  updateColorAndAlpha();
  updateScale();
}

} // end namespace rviz_plugin_covariance

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_covariance::OdometryDisplay, rviz::Display)
