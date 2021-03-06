#ifndef GUI_PLANNINGWIDGET_TOOL_H
#define GUI_PLANNINGWIDGET_TOOL_H

class QContextMenuEvent;
class QGraphicsScene;
class QKeyEvent;
class QMouseEvent;
class QPointF;

namespace gui
{
  namespace planningwidget
  {
    class Context;

    /**
     * @brief The Tool class is the base class of all tools used to interact in special ways with the planning board
     */
    class Tool
    {
    public:
      Tool();
      virtual ~Tool() {}

      // Load and update routines

      /**
       * @brief init is called when the tool is added to the reservation widget
       * This method is used by the reservation widget to tell the tool the information needed to draw and paint on the
       * canvas. This method is called only once during the tool's lifetime
       */
      virtual void init(Context& context) = 0;

      /**
       * @brief load is called before the tool is activated
       */
      virtual void load() = 0;

      /**
       * @brief unload is called before the tool is deactivated.
       * In this method the tool should clear all of the temporary items it may have allocated and added to the scene.
       */
      virtual void unload() = 0;

      /**
       * @brief updateLayout is called whenever the layout of the board changes.
       * A layout change might be triggered e.g. by a reordering or filtering of the rows.
       */
      virtual void updateLayout() = 0;

      // Event handling routines

      virtual void mousePressEvent(QMouseEvent* event, const QPointF& position) {}
      virtual void mouseReleaseEvent(QMouseEvent* event, const QPointF& position) {}
      virtual void mouseMoveEvent(QMouseEvent* event, const QPointF& position) {}
      virtual void contextMenuEvent(QContextMenuEvent* event) {}
      virtual void keyPressEvent(QKeyEvent* event) {}
    };

  } // namespace planningwidget
} // namespace gui

#endif // GUI_PLANNINGWIDGET_TOOL_H
