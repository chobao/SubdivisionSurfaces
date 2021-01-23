#include "visualization/GLViewer.h"
#include "base/model.h"


int main() {
    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    Model model;
    viewer.SetUp(800, 600);
    viewer.Run();
}