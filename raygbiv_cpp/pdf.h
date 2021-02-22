#pragma once

#ifndef PDF_H
#define PDF_H

class pdf
{
  public:
    virtual ~pdf() {}

    // evaluate pdf at this vec3
    virtual float value(const vec3& direction) const = 0;

    // generate a random sample vec3 that conforms to this pdf distribution
    virtual vec3 generate() const = 0;
};

class cosine_pdf : public pdf
{
  public:
    cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

    virtual float value(const vec3& direction) const override
    {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine / pi;
    }

    virtual vec3 generate() const override { return uvw.local(random_cosine_direction()); }

  public:
    onb uvw;
};

class hittable_pdf : public pdf
{
  public:
    hittable_pdf(shared_ptr<hittable> p, const point3& origin)
      : ptr(p)
      , o(origin)
    {}

    virtual float value(const vec3& direction) const override { return ptr->pdf_value(o, direction); }

    virtual vec3 generate() const override { return ptr->random(o); }

  public:
    point3 o;
    shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf
{
  public:
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1)
    {
        p[0] = p0;
        p[1] = p1;
    }

    virtual float value(const vec3& direction) const override
    {
        return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
    }

    virtual vec3 generate() const override
    {
        if (random_float() < 0.5f)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

  public:
    shared_ptr<pdf> p[2];
};

#endif
