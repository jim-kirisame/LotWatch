package org.lotlab.lotwatch;

import android.app.Fragment;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import java.util.ArrayList;
import java.util.List;

import lecho.lib.hellocharts.model.Axis;
import lecho.lib.hellocharts.model.Line;
import lecho.lib.hellocharts.model.LineChartData;
import lecho.lib.hellocharts.model.PieChartData;
import lecho.lib.hellocharts.model.PointValue;
import lecho.lib.hellocharts.model.SliceValue;
import lecho.lib.hellocharts.util.ChartUtils;
import lecho.lib.hellocharts.view.LineChartView;
import lecho.lib.hellocharts.view.PieChartView;

/**
 * Created by jiang on 2017-04-02.
 */

public class SportFragment extends Fragment {
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.frag_sport, container, false);
        //onActivityCreated();
        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        Button btn = (Button) getView().findViewById(R.id.button);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DrawTestData();
            }
        });

    }

    public void DrawTestData()
    {
        LineChartView chart = (LineChartView) getView().findViewById(R.id.sportChart);
        PieChartView pchart = (PieChartView) getView().findViewById(R.id.sportChartToday);

        List<PointValue> values = new ArrayList<PointValue>();
        for (int i = 0; i < 10; ++i) {
            values.add(new PointValue(i, (float)Math.random() * 100 + 15));
        }


        //In most cased you can call data model methods in builder-pattern-like manner.
        Line line = new Line(values).setColor(Color.BLUE).setCubic(true);
        line.setHasLabels(true);
        line.setFilled(true);
        List<Line> lines = new ArrayList<Line>();
        lines.add(line);

        LineChartData data = new LineChartData();
        data.setLines(lines);
        data.setAxisXBottom(new Axis().setName("日期"));
        data.setAxisYLeft(new Axis().setName("步数"));
        chart.setLineChartData(data);



        List<SliceValue> valuespie = new ArrayList<SliceValue>();
        int total = 0;
        for (int i = 0; i < 3; ++i) {
            float ran = (float)Math.random() * 30 + 15;
            total += (int)ran;
            SliceValue sliceValue = new SliceValue(ran , ChartUtils.pickColor());
            valuespie.add(sliceValue);
        }
        PieChartData pdata = new PieChartData(valuespie);
        pdata.setHasLabels(true);
        pdata.setHasLabelsOnlyForSelected(true);
        pdata.setHasLabelsOutside(false);
        pdata.setHasCenterCircle(true);
        pdata.setCenterText1(Integer.toString(total) );

        pchart.setPieChartData(pdata);

    }
}
