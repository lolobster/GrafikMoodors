#ifndef __RENDERER_H__
#define __RENDERER_H__

namespace renderer {
  void Render(void);
  void Init(GLFWwindow* w);
  void Uninit(void);
  void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
}

#endif
