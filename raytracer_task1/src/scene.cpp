#include "scene.h"

#include <float.h>

#include "directionallight.h"
#include "light.h"
#include "renderable.h"
#include "mesh.h"

//==============================================================================
namespace cg {
    //==============================================================================
    Scene::Scene(const Scene &other) : m_background_color{other.m_background_color} {
        for (const auto &renderable: other.m_renderables) {
            m_renderables.push_back(renderable->clone());
        }
        for (const auto &light_source: other.m_light_sources) {
            m_light_sources.push_back(light_source->clone());
        }
    }

    //------------------------------------------------------------------------------
    Scene &Scene::operator=(const Scene &other) {
        for (const auto &renderable: other.m_renderables) {
            m_renderables.push_back(renderable->clone());
        }
        for (const auto &light_source: other.m_light_sources) {
            m_light_sources.push_back(light_source->clone());
        }
        m_background_color = other.m_background_color;
        return *this;
    }

    //==============================================================================
    void Scene::insert(const Renderable &r) {
        m_renderables.push_back(r.clone());
    }

    //------------------------------------------------------------------------------
    void Scene::insert(const AssembledRenderable &ar) {
        for (const auto &r: ar.renderables()) {
            m_renderables.push_back(r->clone());
        }
    }

    //------------------------------------------------------------------------------
    void Scene::insert(AssembledRenderable &&ar) {
        for (auto &r: ar.renderables()) { m_renderables.push_back(std::move(r)); }
    }

    //------------------------------------------------------------------------------
    void Scene::insert(const Light &l) {
        m_light_sources.push_back(l.clone());
    }

    //------------------------------------------------------------------------------
    bool Scene::any_intersection(const Ray &r, double min_t, double max_t) const {
        /*
        ============================================================================
         Task 1
        ============================================================================
         Check if the ray intersects any object in the scene. Return true if it does
         and min_t < t < max_t for the t of the intersection. Else return false.
         Hint: you can use a similar approach to closest_intersection
        */
        for (const auto &obj: m_renderables) { // über alle Objekte iterieren
            // auf Schnitt von Strahl mit Objekt prüfen; Abfrage auf Existenz des Parameters t
            if (std::optional<Intersection> any_hit = obj->check_intersection(r, min_t); any_hit.has_value()) {
                // da Existenz von t feststeht, prüfen, ob min_t < t < max_t, falls ja → Treffer
                if (const double t_hit = any_hit.value().t; min_t < t_hit && t_hit < max_t) return true;
            }
        }
        return false;
    }

    //------------------------------------------------------------------------------
    std::optional<Intersection> Scene::closest_intersection(const Ray &r,
                                                            double min_t) const {
        /*
        ============================================================================
         Task 1
        ============================================================================
         Check which objects in the scene intersects with the ray.
         Return the closest (minimal t) intersection with t > min_t (i.e. any
         intersection that is too close to the ray origin is ignored).
        */
        std::optional<Intersection> closest_hit;

        for (const auto &obj: m_renderables) { // über alle Objekte iterieren
            // obj is of type unique_pointer
            // to access methods and members of it, use
            // obj->method_name() or (*obj).method_name()

            // The return type of check_intersection is an
            // std::optional.
            // See eg. https://en.cppreference.com/w/cpp/utility/optional
            // to see how to work with it

            // auf Schnitt von Strahl mit Objekt prüfen; Abfrage auf Existenz des Parameters t
            if (std::optional<Intersection> any_hit = obj->check_intersection(r, min_t); any_hit.has_value()) {
                // da Existenz von t feststeht, prüfen, ob t > min_t
                if (const double t_hit = any_hit.value().t; t_hit > min_t) {
                    // falls noch kein Hit registriert oder derzeitiger Hit näher → neuer nächster Treffer
                    if (!closest_hit.has_value() || t_hit < closest_hit.value().t) closest_hit = any_hit;
                }
            }
        }
        return closest_hit;
    }

    //------------------------------------------------------------------------------
    /// assembles shading, reflection and refraction
    vec3 Scene::shade_closest_intersection(const Ray &incident_ray,
                                           double min_t) const {
        const auto hit = closest_intersection(incident_ray, min_t);
        if (hit) {
            auto renderable = dynamic_cast<const Renderable *>(hit->intersectable);
            auto shaded_color = vec3::zeros();
            // first apply the lighting model for each light source
            for (const auto &light_source: lights()) {
                if (!in_shadow(*light_source, hit->position + 1e-6 * hit->normal)) {
                    shaded_color +=
                            renderable->shade(*light_source, *hit) *
                            (1 - renderable->reflectance() - renderable->refractance());
                    // only a part of the total incoming light is used for shading the
                    // actual object. The rest is reflected or refracted. The fraction
                    // of light that is transported away is given by the reflectance
                    // and refractance constants of the material.
                }
            }

            /*
            ============================================================================
             Task 3
            ============================================================================
             Implement the calculation of the color that is showing in the reflection on
             this object.
             Remember that ray tracing is recursive.
            */
            if (renderable->reflectance() > 0 &&
                incident_ray.num_reflections() < max_num_reflections) {
                vec3 reflected_color = vec3::zeros();

                shaded_color +=
                        reflected_color *
                        renderable->sample_reflective_color(hit->uv) *
                        renderable->reflectance();
            }

            /*
            ============================================================================
             Task 3
            ============================================================================
             Implement refraction analogously to reflection above.
            */
            if (renderable->refractance() > 0 &&
                incident_ray.num_reflections() < max_num_reflections) {
                vec3 refracted_color = vec3::zeros();

                shaded_color +=
                        refracted_color *
                        renderable->sample_refractive_color(hit->uv) *
                        renderable->refractance();
            }
            return shaded_color;
        }
        return m_background_color;
    }

    //------------------------------------------------------------------------------
    bool Scene::in_shadow(const Light &light_source, const vec3 &position) const {
        /*
        ============================================================================
         Task 1
        ============================================================================
         Check if light from light_source reaches position (i.e. if position is in
         the shadow of light_source). Return true if it is in shadow, false else.

         Hint: Have a look at the available methods of a Light object (see light.h).
         You can use any_intersection to implement this method.
         You can assume any object to be opaque for the shadow calculation (so even
         transparent objects cast a shadow).
        */
        vec3 direction;
        double max_t;

        if (light_source.distance_to(position) == std::numeric_limits<double>::max()) { // gerichtetes Licht
            direction = -light_source.light_direction_to(position); // '-' → Richtung von Punkt zu Licht
            max_t = std::numeric_limits<double>::max(); // Licht "unendlich" weit entfernt
        }
        else { // Punktlicht
            direction = -light_source.light_direction_to(position); // für + -> org: light_source.pos()
            max_t = light_source.distance_to(position); // Distanz in beide Richtungen identisch
        }
        // Bei Treffer liegt Objekt vor Lichtstrahl → Punkt im Schatten
        return any_intersection(Ray(position, direction), 0, max_t);
    }

    /*
    ============================================================================
     Task 1 - Bonus
    ============================================================================
    Find the error in check_intersection in plane.cpp. Describe the error and
    propose a solution to fix it (either as (pseudo)-code or in words).

    Answer (German or English):
    Bei der Berechnung des Parameters t wurde die Formel aufgeteilt, sodass zunächst der Nenner ermittelt und
    anschließend in den Rest der Formel eingesetzt wird. Der Nenner berechnet sich durch das Skalarprodukt der
    Rückrichtung des Strahls und dem Normalenvektor der Ebene. Es kann passieren, dass der betrachtete Strahl parallel
    zur Ebene verläuft oder ganz in ihr liegt, sodass das Skalarprodukt mit dem Normalenvektor 0 ergäbe, da sie
    senkrecht zueinander stünden. In diesem Fall würde im darauffolgenden Schritt versucht werden, durch 0 zu teilen,
    was nicht definiert ist und somit zu undefiniertem Verhalten bis hin zum Absturz des Programms führen wird.

    → double denom = ...; if (denom == 0) return {}; double t = ...;
    */

    //==============================================================================
} // namespace cg
//==============================================================================
