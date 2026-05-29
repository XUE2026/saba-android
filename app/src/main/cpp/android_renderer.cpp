#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <cmath>

#define LOG_TAG "SabaRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace saba {
namespace android {

struct CameraState {
    float eyeX = 0.0f, eyeY = 15.0f, eyeZ = 50.0f;
    float centerX = 0.0f, centerY = 10.0f, centerZ = 0.0f;
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    float fov = 45.0f;
    float nearClip = 1.0f;
    float farClip = 2000.0f;
};

struct ViewportState {
    int width = 0;
    int height = 0;
    float aspect = 1.0f;
};

struct TouchState {
    std::mutex mutex;
    std::vector<float> x;
    std::vector<float> y;
    std::vector<int> ids;
    int action = -1;
    float lastPinchDist = 0.0f;
};

class AndroidRenderer {
public:
    AndroidRenderer() = default;
    ~AndroidRenderer() = default;

    bool Initialize() {
        glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_initialized = true;
        LOGI("AndroidRenderer initialized");
        return true;
    }

    void Resize(int width, int height) {
        m_viewport.width = width;
        m_viewport.height = height;
        m_viewport.aspect = static_cast<float>(width) / static_cast<float>(height);
        glViewport(0, 0, width, height);
    }

    void Draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_viewport.width <= 0 || m_viewport.height <= 0) return;

        glm::mat4 projection = glm::perspective(
            glm::radians(m_camera.fov),
            m_viewport.aspect,
            m_camera.nearClip,
            m_camera.farClip
        );

        glm::mat4 view = glm::lookAt(
            glm::vec3(m_camera.eyeX, m_camera.eyeY, m_camera.eyeZ),
            glm::vec3(m_camera.centerX, m_camera.centerY, m_camera.centerZ),
            glm::vec3(m_camera.upX, m_camera.upY, m_camera.upZ)
        );

        DrawGrid(projection, view);
    }

    void DrawGrid(const glm::mat4& proj, const glm::mat4& view) {
        static bool gridInit = false;
        static GLuint gridVAO = 0, gridVBO = 0;
        static GLuint gridProgram = 0;

        if (!gridInit) {
            const char* vertSrc = "#version 300 es\n"
                "layout(location = 0) in vec3 aPos;\n"
                "uniform mat4 uMVP;\n"
                "void main() { gl_Position = uMVP * vec4(aPos, 1.0); }";

            const char* fragSrc = "#version 300 es\n"
                "precision highp float;\n"
                "out vec4 fragColor;\n"
                "uniform vec4 uColor;\n"
                "void main() { fragColor = uColor; }";

            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vs, 1, &vertSrc, nullptr);
            glCompileShader(vs);

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fs, 1, &fragSrc, nullptr);
            glCompileShader(fs);

            gridProgram = glCreateProgram();
            glAttachShader(gridProgram, vs);
            glAttachShader(gridProgram, fs);
            glLinkProgram(gridProgram);

            glDeleteShader(vs);
            glDeleteShader(fs);

            std::vector<float> vertices;
            for (int i = -50; i <= 50; i++) {
                vertices.push_back(static_cast<float>(i));
                vertices.push_back(0.0f);
                vertices.push_back(-50.0f);
                vertices.push_back(static_cast<float>(i));
                vertices.push_back(0.0f);
                vertices.push_back(50.0f);
                vertices.push_back(-50.0f);
                vertices.push_back(0.0f);
                vertices.push_back(static_cast<float>(i));
                vertices.push_back(50.0f);
                vertices.push_back(0.0f);
                vertices.push_back(static_cast<float>(i));
            }

            glGenVertexArrays(1, &gridVAO);
            glGenBuffers(1, &gridVBO);
            glBindVertexArray(gridVAO);
            glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);

            gridInit = true;
        }

        glUseProgram(gridProgram);
        glm::mat4 mvp = proj * view;
        glUniformMatrix4fv(glGetUniformLocation(gridProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform4f(glGetUniformLocation(gridProgram, "uColor"), 0.5f, 0.5f, 0.5f, 0.5f);

        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, 404);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    bool IsInitialized() const { return m_initialized; }
    CameraState& GetCamera() { return m_camera; }
    ViewportState& GetViewport() { return m_viewport; }
    TouchState& GetTouchState() { return m_touch; }

private:
    bool m_initialized = false;
    CameraState m_camera;
    ViewportState m_viewport;
    TouchState m_touch;
};

static AndroidRenderer s_renderer;

} // namespace android
} // namespace saba

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeInitRenderer(JNIEnv* env, jclass clazz) {
    return saba::android::s_renderer.Initialize() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeRenderFrame(JNIEnv* env, jclass clazz) {
    saba::android::s_renderer.Draw();
}

JNIEXPORT void JNICALL
Java_com_saba_viewer_SabaNativeSurfaceView_nativeSetCamera(JNIEnv* env, jclass clazz,
                                                           jfloat eyeX, jfloat eyeY, jfloat eyeZ,
                                                           jfloat centerX, jfloat centerY, jfloat centerZ) {
    auto& cam = saba::android::s_renderer.GetCamera();
    cam.eyeX = eyeX; cam.eyeY = eyeY; cam.eyeZ = eyeZ;
    cam.centerX = centerX; cam.centerY = centerY; cam.centerZ = centerZ;
}

} // extern "C"